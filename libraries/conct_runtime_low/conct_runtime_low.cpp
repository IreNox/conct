#include "conct_runtime_low.h"

#include "conct_buffered_value_builder.h"
#include "conct_device.h"
#include "conct_functions.h"
#include "conct_memory.h"
#include "conct_port.h"
#include "conct_proxy.h"
#include "conct_reader.h"
#include "conct_runtime.h"
#include "conct_writer.h"

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
#	include <Curve25519.h>
#	include <RNG.h>

#	if CONCT_ENABLED( CONCT_PLATFORM_ARDUINO )
#		include "Entropy.h"
#	endif
#endif

#define CONCT_RUNTIME_TRACES CONCT_OFF // CONCT_IF( CONCT_DISABLED( CONCT_ENVIRONMENT_SIMULATOR ) )

#if CONCT_ENABLED( CONCT_RUNTIME_TRACES )
#	include <arduino.h>

#	define CONCT_RUNTIME_PRINT			Serial.print
#	define CONCT_RUNTIME_PRINTLINE		Serial.println
#else
#	define CONCT_RUNTIME_PRINT( ... )
#	define CONCT_RUNTIME_PRINTLINE( ... )
#endif

namespace conct
{
	void RuntimeLow::setup( Device* pDevice )
	{
		m_pDevice				= pDevice;
		m_state					= State_First;
		m_stateValue			= 0u;
		m_workingDataOffset		= 0u;
#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		m_encrypted				= false;
#	if CONCT_ENABLED( CONCT_PLATFORM_ARDUINO )
		Entropy.initialize();
#	endif
#endif

	}

	void RuntimeLow::processPort( Port* pPort )
	{
		{
			uintreg endpointId;
			while( pPort->popConnectionReset( endpointId ) )
			{
				m_workingDataOffset = 0u;
				setState( State_First );
			}
		}

		if( m_state == State_ExecuteCommand )
		{
			executeCommand();
		}

		if( m_state == State_Send )
		{
			sendData( pPort );
		}
		else
		{
			readData( pPort );
		}
	}

	void* RuntimeLow::getWorkingData()
	{
		return m_workingData + m_workingDataOffset;
	}

	uintreg RuntimeLow::getRemainingWorkingData() const
	{
		return sizeof( m_workingData ) - m_workingDataOffset;
	}

	void RuntimeLow::setState( State state, uintreg stateValue /* = 0u */ )
	{
		m_state			= state;
		m_stateValue	= stateValue;
	}

	void RuntimeLow::readData( Port* pPort )
	{
		Reader reader;
		uintreg endpointId;
		if( !pPort->openReceived( reader, endpointId ) )
		{
			return;
		}

		readData( reader );

		pPort->closeReceived( reader, endpointId );
	}

	void RuntimeLow::readData( Reader& reader )
	{
		ReadResult result = ReadResult_Ok;
		while( !reader.isEnd() )
		{
			switch( m_state )
			{
#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
			case State_ReadCryptoHeader:
				result = readCryptoHeader( reader );
				break;
#endif

			case State_ReadBaseHeader:
				result = readBaseHeader( reader );
				break;

			case State_ReadAddresses:
				result = readAddresses( reader );
				break;

			case State_ReadPayload:
				result = readPayload( reader );
				break;

			default:
				// unsupported state
				return;
			}

			if( result == ReadResult_Error )
			{
				CONCT_RUNTIME_PRINTLINE( "Receive error" );
			}
			else if( result == ReadResult_WaitingData )
			{
				CONCT_ASSERT( reader.isEnd() );
				break;
			}
		}
	}

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
	RuntimeLow::ReadResult RuntimeLow::readCryptoHeader( Reader& reader )
	{
		if( m_encrypted )
		{
			MessageCryptoHeader& cryptoHeader = *static_cast< MessageCryptoHeader* >( getWorkingData() );
			m_stateValue += reader.readStruct< MessageCryptoHeader >( cryptoHeader, m_stateValue );
			if( m_stateValue < sizeof( cryptoHeader ) )
			{
				return ReadResult_WaitingData;
			}

			m_crypto.setKey( m_cryptoKey.data, sizeof( m_cryptoKey ) );
			m_crypto.setIV( cryptoHeader.cryptoIV.data, sizeof( cryptoHeader.cryptoIV ) );
			m_crypto.setCounter( cryptoHeader.cryptoCounter.data, sizeof( cryptoHeader.cryptoCounter ) );
		}

		setState( State_ReadBaseHeader );
		return ReadResult_Ok;
	}
#endif

	RuntimeLow::ReadResult RuntimeLow::readBaseHeader( Reader& reader )
	{
		MessageBaseHeader& baseHeader = *static_cast< MessageBaseHeader* >( getWorkingData() );
		m_stateValue += reader.readStruct< MessageBaseHeader >( baseHeader, m_stateValue );
		if( m_stateValue < sizeof( baseHeader ) )
		{
			return ReadResult_WaitingData;
		}

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		if( m_encrypted )
		{
			uint8* pBaseHeader = (uint8*)&baseHeader;
			m_crypto.decrypt( pBaseHeader, pBaseHeader, sizeof( baseHeader ) );
		}
#endif

		if( baseHeader.destinationHops > 1u )
		{
			CONCT_RUNTIME_PRINTLINE( "Received routing package" );
			sendErrorResponse( MessageType_ErrorResponse, ResultId_Unsupported );
			return ReadResult_Error;
		}

		if( baseHeader.sourceHops + 1u > sizeof( m_workingData ) )
		{
			CONCT_RUNTIME_PRINTLINE( "Received package too large" );
			sendErrorResponse( MessageType_ErrorResponse, ResultId_OutOfMemory );
			return ReadResult_Error;
		}

		m_workingDataOffset			= 0u;
		m_playloadSize				= baseHeader.payloadSize;
		m_destinationAddressSize	= baseHeader.sourceHops;
		m_messageType				= baseHeader.messageType;
		m_commandId					= baseHeader.commandId;
		m_result					= baseHeader.messageResult;

		setState( State_ReadAddresses );
		return ReadResult_Ok;
	}

	RuntimeLow::ReadResult RuntimeLow::readAddresses( Reader& reader )
	{
		uint8* pAddress = (uint8*)getWorkingData();
		m_stateValue += reader.readData( pAddress, m_destinationAddressSize + 1u, m_stateValue );
		if( m_stateValue < m_destinationAddressSize + 1u )
		{
			return ReadResult_WaitingData;
		}

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		if( m_encrypted )
		{
			m_crypto.decrypt( pAddress, pAddress, m_destinationAddressSize + 1u );
		}
#endif

		m_workingDataOffset += m_destinationAddressSize;

		if( m_playloadSize > getRemainingWorkingData() )
		{
			CONCT_RUNTIME_PRINTLINE( "Received package too large" );
			sendErrorResponse( MessageType_ErrorResponse, ResultId_OutOfMemory );
			return ReadResult_Error;
		}

		setState( State_ReadPayload );
		return ReadResult_Ok;
	}

	RuntimeLow::ReadResult RuntimeLow::readPayload( Reader& reader )
	{
		uint8* pPayload = (uint8*)getWorkingData();
		m_stateValue += reader.readData( pPayload, (uintreg)m_playloadSize, m_stateValue );;
		if( m_stateValue < m_playloadSize )
		{
			return ReadResult_WaitingData;
		}

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		if( m_encrypted )
		{
			m_crypto.decrypt( pPayload, pPayload, m_playloadSize );
		}
#endif

		m_workingDataOffset += m_playloadSize;

		setState( State_ExecuteCommand );
		return ReadResult_Ok;
	}

	void RuntimeLow::executeCommand()
	{
		m_workingDataOffset = sizeof( MessageBaseHeader ) + 1u + m_destinationAddressSize;
#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		if( m_encrypted )
		{
			m_workingDataOffset += sizeof( MessageCryptoHeader );
		}
#endif

		switch( m_messageType )
		{
		case MessageType_PingRequest:
			sendPingResponse();
			break;

		case MessageType_GetPropertyRequest:
			{
				BasicValueBuilder valueBuilder( getWorkingData(), getRemainingWorkingData() );
				{
					const GetPropertyRequest& request = *reinterpret_cast< const GetPropertyRequest* >( m_workingData + m_destinationAddressSize );

					const LocalInstance* pInstance = m_pDevice->getInstance( request.instanceId );
					if( pInstance == nullptr )
					{
						CONCT_RUNTIME_PRINTLINE( "GetPropery: No such instance" );
						sendErrorResponse( MessageType_GetPropertyResponse, ResultId_NoSuchInstance );
						return;
					}

					if( !pInstance->pProxy->getProperty( valueBuilder, pInstance->pInstance, request.nameCrc ) )
					{
						CONCT_RUNTIME_PRINTLINE( "GetPropery: No such field" );
						sendErrorResponse( MessageType_GetPropertyResponse, ResultId_NoSuchField );
						return;
					}
				}

				sendResponse( MessageType_GetPropertyResponse, valueBuilder.getValue(), valueBuilder.getValueSize() );
			}
			break;

		case MessageType_SetPropertyRequest:
			{
				const SetPropertyRequest& request = *reinterpret_cast< const SetPropertyRequest* >( m_workingData + m_destinationAddressSize );

				const LocalInstance* pInstance = m_pDevice->getInstance( request.instanceId );
				if( pInstance == nullptr )
				{
					CONCT_RUNTIME_PRINTLINE( "SetPropery: No such instance" );
					sendErrorResponse( MessageType_SetPropertyResponse, ResultId_NoSuchInstance );
					return;
				}

				if( !pInstance->pProxy->setProperty( pInstance->pInstance, request.nameCrc, request.value ) )
				{
					CONCT_RUNTIME_PRINTLINE( "SetPropery: No such field" );
					sendErrorResponse( MessageType_SetPropertyResponse, ResultId_NoSuchField );
					return;
				}

				sendResponse( MessageType_SetPropertyResponse, nullptr, 0u );
			}
			break;

		case MessageType_CallFunctionRequest:
			{
				BasicValueBuilder valueBuilder( getWorkingData(), getRemainingWorkingData() );
				{
					const CallFunctionRequest& request = *reinterpret_cast< const CallFunctionRequest* >( m_workingData + m_destinationAddressSize );

					const LocalInstance* pInstance = m_pDevice->getInstance( request.instanceId );
					if( pInstance == nullptr )
					{
						CONCT_RUNTIME_PRINTLINE( "CallFunction: No such instance" );
						sendErrorResponse( MessageType_GetPropertyResponse, ResultId_NoSuchInstance );
						return;
					}

					if( !pInstance->pProxy->callFunction( valueBuilder, pInstance->pInstance, request.nameCrc, request.arguments.toView() ) )
					{
						CONCT_RUNTIME_PRINTLINE( "CallFunction: No such function" );
						sendErrorResponse( MessageType_GetPropertyResponse, ResultId_NoSuchField );
						return;
					}
				}

				sendResponse( MessageType_GetPropertyResponse, valueBuilder.getValue(), valueBuilder.getValueSize() );
			}
			break;

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		case MessageType_CryptoHandshake:
			{
				if( m_result != ResultId_Success )
				{
					CONCT_RUNTIME_PRINTLINE( "Received key exchange failed" );
					m_encrypted = false;
					return;
				}

				CryptoHandshake* pCryptoHandshake		= (CryptoHandshake*)(m_workingData + m_destinationAddressSize);
				CryptoHandshake* pNewCryptoHandshake	= &pCryptoHandshake[ 1u ];
				CONCT_ASSERT( (uintreg)&pNewCryptoHandshake[ 1u ] < (uintreg)&m_workingData[ sizeof( m_workingData ) ] );

				Curve25519::dh1( pNewCryptoHandshake->publicKey.data, m_cryptoKey.data );

				if( !Curve25519::dh2( pCryptoHandshake->publicKey.data, m_cryptoKey.data ) )
				{
					CONCT_RUNTIME_PRINTLINE( "Key exchange failed" );
					sendErrorResponse( MessageType_CryptoHandshake, ResultId_KeyExchangeFailed );
					return;
				}

				m_cryptoKey = pCryptoHandshake->publicKey;

				sendResponse( MessageType_CryptoHandshake, pNewCryptoHandshake, sizeof( *pNewCryptoHandshake ) );

				m_encrypted = true;
			}
			break;
#endif

		default:
			CONCT_RUNTIME_PRINTLINE( "Received unsupported message type." );
			sendErrorResponse( MessageType_ErrorResponse, ResultId_Unsupported );
			break;
		}
	}

	void RuntimeLow::sendPingResponse()
	{
		m_result = ResultId_Success;
		sendResponse( MessageType_PingResponse, nullptr, 0u );
	}

	void RuntimeLow::sendErrorResponse( MessageType responseType, ResultId result )
	{
		m_result = result;
		sendResponse( responseType, nullptr, 0u );
	}

	void RuntimeLow::sendResponse( MessageType responseType, const void* pData, uintreg dataLength )
	{
		uintreg packetSize = sizeof( MessageBaseHeader ) + 1u + m_destinationAddressSize + dataLength;
#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		if( m_encrypted )
		{
			packetSize += sizeof( MessageCryptoHeader );
		}
#endif

		if( sizeof( m_workingData ) < packetSize )
		{
			CONCT_RUNTIME_PRINTLINE( "Respose too large." );
			sendErrorResponse( MessageType_ErrorResponse, ResultId_OutOfMemory );
			return;
		}

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		MessageCryptoHeader* pCryptoHeader = (MessageCryptoHeader*)m_workingData;
		if( m_encrypted )
		{
			m_workingDataOffset = sizeof( *pCryptoHeader );
		}
		else
#endif
		{
			m_workingDataOffset = 0u;
		}

		MessageBaseHeader* pBaseHeader = (MessageBaseHeader*)getWorkingData();
		DeviceId* pSourceAddress = (DeviceId*)&pBaseHeader[ 1u ];
		DeviceId* pDestinationAddress = &pSourceAddress[ 1u ];
		uint8* pPayload = &pDestinationAddress[ m_destinationAddressSize ];

		CONCT_ASSERT( (uintreg)pPayload <= (uintreg)pData );
		memory::copyOverlapping( pPayload, pData, dataLength );
		memory::copyOverlapping( pDestinationAddress, m_workingData, m_destinationAddressSize );

		pSourceAddress[ 0u ] = 1u;

		pBaseHeader->sourceHops			= 1u;
		pBaseHeader->destinationHops	= m_destinationAddressSize;
		pBaseHeader->payloadSize		= uint16( dataLength );
		pBaseHeader->commandId			= m_commandId;
		pBaseHeader->messageType		= responseType;
		pBaseHeader->messageResult		= m_result;

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		if( m_encrypted )
		{
			RNG.rand( (uint8*)pCryptoHeader, sizeof( *pCryptoHeader ) );

			m_crypto.setKey( m_cryptoKey.data, sizeof( m_cryptoKey ) );
			m_crypto.setIV( pCryptoHeader->cryptoIV.data, sizeof( pCryptoHeader->cryptoIV ) );
			m_crypto.setCounter( pCryptoHeader->cryptoCounter.data, sizeof( pCryptoHeader->cryptoCounter ) );

			uint8* pPayload = m_workingData + sizeof( MessageCryptoHeader );
			uintreg payloadSize = packetSize - sizeof( MessageCryptoHeader );
			m_crypto.encrypt( pPayload, pPayload, payloadSize );
		}
#endif

		m_workingDataOffset = 0u;
		setState( State_Send, packetSize );
	}

	void RuntimeLow::sendData( Port* pPort )
	{
		Writer writer;
		if( !pPort->openSend( writer, m_stateValue, 0u ) )
		{
			return;
		}

		while( !writer.isEnd() && m_stateValue > 0u )
		{
			const uintreg writtenBytes = writer.writeData( getWorkingData(), m_stateValue );
			m_stateValue -= writtenBytes;
			m_workingDataOffset += writtenBytes;
		}

		pPort->closeSend( writer, 0u );

		if( m_stateValue == 0u )
		{
			m_workingDataOffset = 0u;
			setState( State_First );
		}
	}
}