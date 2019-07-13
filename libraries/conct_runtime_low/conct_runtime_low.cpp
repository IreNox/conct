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

#include <arduino.h>

#define CONCT_RUNTIME_TRACES CONCT_ON

#if CONCT_ENABLED( CONCT_RUNTIME_TRACES )
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
		m_state					= State_ReadBaseHeader;
		m_stateValue			= 0u;
		m_workingDataOffset		= 0u;
	}

	void RuntimeLow::processPort( Port* pPort )
	{
		uintreg endpointId;
		while( pPort->popConnectionReset( endpointId ) )
		{
			m_workingDataOffset = 0u;
			setState( State_ReadBaseHeader );
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

	RuntimeLow::ReadResult RuntimeLow::readBaseHeader( Reader& reader )
	{
		MessageBaseHeader& baseHeader = *static_cast< MessageBaseHeader* >( getWorkingData() );
		m_stateValue += reader.readStruct< MessageBaseHeader >( baseHeader, ( uintreg )m_stateValue );
		if( m_stateValue < sizeof( baseHeader ) )
		{
			return ReadResult_WaitingData;
		}

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
		m_result					= ResultId_Success;

		setState( State_ReadAddresses );
		return ReadResult_Ok;
	}

	RuntimeLow::ReadResult RuntimeLow::readAddresses( Reader& reader )
	{
		m_stateValue += reader.readData( getWorkingData(), m_destinationAddressSize + 1u, ( uintreg )m_stateValue );
		if( m_stateValue < m_destinationAddressSize + 1u )
		{
			return ReadResult_WaitingData;
		}

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
		m_stateValue += reader.readData( getWorkingData(), ( uintreg )m_playloadSize, ( uintreg )m_stateValue );
		if( m_stateValue < m_playloadSize )
		{
			return ReadResult_WaitingData;
		}

		m_workingDataOffset += m_playloadSize;

		setState( State_ExecuteCommand );
		return ReadResult_Ok;
	}

	void RuntimeLow::executeCommand()
	{
		m_workingDataOffset = sizeof( MessageBaseHeader ) + 1u + m_destinationAddressSize;

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
		const uintreg packetSize = sizeof( MessageBaseHeader ) + 1u + m_destinationAddressSize + dataLength;
		if( sizeof( m_workingData ) < packetSize )
		{
			CONCT_RUNTIME_PRINTLINE( "Respose too large." );
			sendErrorResponse( MessageType_ErrorResponse, ResultId_OutOfMemory );
			return;
		}

		MessageBaseHeader* pBaseHeader = ( MessageBaseHeader* )m_workingData;
		DeviceId* pSourceAddress = (DeviceId*)&pBaseHeader[ 1u ];
		DeviceId* pDestinationAddress = &pSourceAddress[ 1u ];
		uint8* pPayload = &pDestinationAddress[ m_destinationAddressSize ];

		memory::copy( pDestinationAddress, m_workingData, m_destinationAddressSize );
		memory::copy( pPayload, pData, dataLength );

		pSourceAddress[ 0u ] = 1u;

		pBaseHeader->sourceHops			= 1u;
		pBaseHeader->destinationHops	= m_destinationAddressSize;
		pBaseHeader->payloadSize		= uint16( dataLength );
		pBaseHeader->commandId			= m_commandId;
		pBaseHeader->messageType		= responseType;
		pBaseHeader->messageResult		= m_result;

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
			setState( State_ReadBaseHeader );
		}
	}
}