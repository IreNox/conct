#include "conct_runtime_low.h"

#include "conct_device.h"
#include "conct_functions.h"
#include "conct_memory.h"
#include "conct_port.h"
#include "conct_proxy.h"
#include "conct_reader.h"
#include "conct_runtime.h"
#include "conct_writer.h"

namespace conct
{
	void RuntimeLow::setup( Device* pDevice )
	{
		pDevice->getInstances( m_instances );

		m_state					= State_ReadBaseHeader;
		m_stateValue			= 0u;
		m_workingDataOffset		= 0u;
	}

	void RuntimeLow::processPort( Port* pPort )
	{
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

	uint8 RuntimeLow::getRemainingWorkingData() const
	{
		return sizeof( m_workingData ) - m_workingDataOffset;
	}

	const LocalInstance* RuntimeLow::findInstanceById( InstanceId instaceId )
	{
		for( uint8 i = 0u; i < m_instances.getCount(); ++i )
		{
			if( m_instances[ i ].id == instaceId )
			{
				return &m_instances[ i ];
			}
		}

		return nullptr;
	}

	const LocalInstance* RuntimeLow::findInstanceByType( TypeCrc typeCrc )
	{
		for( uint8 i = 0u; i < m_instances.getCount(); ++i )
		{
			if( m_instances[ i ].pProxy->getTypeCrc() == typeCrc )
			{
				return &m_instances[ i ];
			}
		}

		return nullptr;
	}

	void RuntimeLow::setState( State state, uint16 stateValue /* = 0u */ )
	{
		m_state			= state;
		m_stateValue	= stateValue;
	}

	void RuntimeLow::readData( Port* pPort )
	{
		Reader reader;
		DeviceId deviceId;
		if( !pPort->openReceived( reader, deviceId ) )
		{
			return;
		}

		readData( reader );

		pPort->closeReceived( reader );
	}

	void RuntimeLow::readData( Reader& reader )
	{
		ReadResult result = ReadResult_Ok;
		while( !reader.isEnd() )
		{
			switch( m_state )
			{
			//case State_ReadUntilNextMessage:
			//	result = readUntilNextMessage( reader );
			//	break;

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
				// ...
			}
			else if( result == ReadResult_WaitingData )
			{
				CONCT_ASSERT( reader.isEnd() );
				break;
			}
		}
	}

	//RuntimeLow::ReadResult RuntimeLow::readUntilNextMessage( Reader& reader )
	//{
	//	uint16& magic = *( uint16* )m_workingData;

	//	if( m_stateValue < 2u )
	//	{
	//		m_stateValue += reader.readShort( magic, uintreg( sizeof( s_messageBaseHeaderMagic ) - m_stateValue ) );

	//		if( m_stateValue <= sizeof( s_messageBaseHeaderMagic ) )
	//		{
	//			return ReadResult_WaitingData;
	//		}

	//		if( magic == s_messageBaseHeaderMagic )
	//		{
	//			setState( State_ReadBaseHeader );
	//			return ReadResult_Ok;
	//		}
	//	}

	//	uint8 nextByte = 0u;
	//	while( reader.readByte( nextByte ) )
	//	{
	//		magic <<= 8u;
	//		magic |= nextByte;

	//		if( magic == s_messageBaseHeaderMagic )
	//		{
	//			setState( State_ReadBaseHeader );
	//			return ReadResult_Ok;
	//		}
	//	}

	//	return ReadResult_WaitingData;
	//}

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
			sendErrorResponse( ResultId_Unsupported );
			return ReadResult_Error;
		}

		if( baseHeader.sourceHops + 1u > sizeof( m_workingData ) )
		{
			sendErrorResponse( ResultId_OutOfMemory );
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
			sendErrorResponse( ResultId_OutOfMemory );
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
		switch( m_messageType )
		{
		case MessageType_PingRequest:
			sendPingResponse();
			break;

		case MessageType_GetInstanceRequest:
			{
				const GetInstanceRequest& request = *reinterpret_cast< const GetInstanceRequest* >( m_workingData + m_destinationAddressSize );

				const LocalInstance* pInstance = findInstanceByType( request.typeCrc );
				if( pInstance == nullptr )
				{
					sendErrorResponse( ResultId_NoSuchInstance );
					return;
				}

				GetInstanceResponse response;
				response.instanceId = pInstance->id;

				sendResponse( MessageType_GetInstanceResponse, &response, sizeof( response ) );
			}
			break;

		case MessageType_GetPropertyRequest:
			{
				Value value;
				{
					const GetPropertyRequest& request = *reinterpret_cast< const GetPropertyRequest* >( m_workingData + m_destinationAddressSize );

					const LocalInstance* pInstance = findInstanceById( request.instanceId );
					if( pInstance == nullptr )
					{
						sendErrorResponse( ResultId_NoSuchInstance );
						return;
					}

					if( !pInstance->pProxy->getProperty( value, pInstance->pInstance, request.name ) )
					{
						sendErrorResponse( ResultId_NoSuchField );
						return;
					}
				}

				sendResponse( MessageType_GetPropertyResponse, &value, sizeof( value ) );
			}
			break;

		case MessageType_SetPropertyRequest:
			{
				const SetPropertyRequest& request = *reinterpret_cast< const SetPropertyRequest* >( m_workingData + m_destinationAddressSize );

				const LocalInstance* pInstance = findInstanceById( request.instanceId );
				if( pInstance == nullptr )
				{
					sendErrorResponse( ResultId_NoSuchInstance );
					return;
				}

				if( !pInstance->pProxy->setProperty( pInstance->pInstance, request.name, request.value ) )
				{
					sendErrorResponse( ResultId_NoSuchField );
					return;
				}

				sendResponse( MessageType_SetPropertyResponse, nullptr, 0u );
			}
			break;

		//case MessageType_CallFunctionRequest:
		//	break;

		default:
			sendErrorResponse( ResultId_Unsupported );
			break;
		}
	}

	void RuntimeLow::sendPingResponse()
	{
		m_messageType	= MessageType_PingResponse;
		m_result		= ResultId_Success;

		sendResponse( MessageType_ErrorResponse, nullptr, 0u );
	}

	void RuntimeLow::sendErrorResponse( ResultId result )
	{
		m_messageType	= MessageType_ErrorResponse;
		m_result		= result;

		sendResponse( MessageType_ErrorResponse, nullptr, 0u );
	}

	void RuntimeLow::sendResponse( MessageType responseType, const void* pData, uint8 dataLength )
	{
		const uintreg packetSize = sizeof( MessageBaseHeader ) + 1u + m_destinationAddressSize + dataLength;
		if( sizeof( m_workingData ) < packetSize )
		{
			sendErrorResponse( ResultId_OutOfMemory );
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
		pBaseHeader->payloadSize		= dataLength;
		pBaseHeader->commandId			= m_commandId;
		pBaseHeader->messageType		= responseType;
		pBaseHeader->messageResult		= m_result;

		m_workingDataOffset = 0u;
		setState( State_Send, packetSize );
	}

	void RuntimeLow::sendData( Port* pPort )
	{
		Writer writer;
		if( !pPort->openSend( writer, m_stateValue, 1u ) )
		{
			return;
		}

		while( !writer.isEnd() && m_stateValue > 0u )
		{
			const uintreg writtenBytes = writer.writeData( getWorkingData(), m_stateValue );
			m_stateValue -= writtenBytes;
			m_workingDataOffset += writtenBytes;
		}

		pPort->closeSend( writer );

		if( m_stateValue == 0u )
		{
			setState( State_ReadBaseHeader );
		}
	}
}