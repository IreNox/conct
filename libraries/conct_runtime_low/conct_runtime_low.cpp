#include "conct_runtime_low.h"

#include "conct_device.h"
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

		if( m_state == State_SendResponse )
		{
			sendData( pPort );
		}
		else
		{
			processData( pPort );
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

	const LocalInstance* RuntimeLow::findInstance( InstanceId instaceId )
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

	void RuntimeLow::setState( State state, uint16 stateValue /* = 0u */ )
	{
		m_state			= state;
		m_stateValue	= stateValue;
	}

	void RuntimeLow::processData( Port* pPort )
	{
		Reader reader;
		DeviceId deviceId;
		if( !pPort->openReceived( reader, deviceId ) )
		{
			return;
		}

		processData( reader );

		pPort->closeReceived( reader );
	}

	void RuntimeLow::processData( Reader& reader )
	{
		ProcessResult result = ProcessResult_Ok;
		while( !reader.isEnd() )
		{
			switch( m_state )
			{
			case State_ReadUntilNextMessage:
				result = processUntilNextMessage( reader );
				break;

			case State_ReadBaseHeader:
				result = processBaseHeader( reader );
				break;

			case State_ReadAddress:
				result = processAddress( reader );
				break;

			case State_ReadMessageHeader:
				result = processMessageHeader( reader );
				break;

			case State_ReadMessage:
				result = processMessage( reader );
				break;

			default:
				// unsupported state
				return;
			}

			if( result == ProcessResult_Error )
			{
				// ...
			}
			else if( result == ProcessResult_WaitingData )
			{
				CONCT_ASSERT( reader.isEnd() );
				break;
			}
		}
	}

	RuntimeLow::ProcessResult RuntimeLow::processUntilNextMessage( Reader& reader )
	{
		uint16& magic = *( uint16* )m_workingData;

		if( m_stateValue < 2u )
		{
			m_stateValue += reader.readShort( magic, uintreg( sizeof( s_messageBaseHeaderMagic ) - m_stateValue ) );

			if( m_stateValue <= sizeof( s_messageBaseHeaderMagic ) )
			{
				return ProcessResult_WaitingData;
			}

			if( magic == s_messageBaseHeaderMagic )
			{
				setState( State_ReadBaseHeader );
				return ProcessResult_Ok;
			}
		}

		uint8 nextByte = 0u;
		while( reader.readByte( nextByte ) )
		{
			magic <<= 8u;
			magic |= nextByte;

			if( magic == s_messageBaseHeaderMagic )
			{
				setState( State_ReadBaseHeader );
				return ProcessResult_Ok;
			}
		}

		return ProcessResult_WaitingData;
	}

	RuntimeLow::ProcessResult RuntimeLow::processBaseHeader( Reader& reader )
	{
		MessageBaseHeader& baseHeader = *static_cast< MessageBaseHeader* >( getWorkingData() );
		m_stateValue += reader.readStruct< MessageBaseHeader >( baseHeader, ( uintreg )m_stateValue );
		if( m_stateValue < sizeof( baseHeader ) )
		{
			return ProcessResult_WaitingData;
		}

		if( baseHeader.destinationHops > 1u )
		{
			sendErrorResponse( ResultId_Unsupported );
			return ProcessResult_Error;
		}

		if( baseHeader.sourceHops > sizeof( m_workingData ) )
		{
			sendErrorResponse( ResultId_OutOfMemory );
			return ProcessResult_Error;
		}

		m_workingDataOffset			= 0u;
		m_playloadSize				= baseHeader.payloadSize;
		m_destinationAddressSize	= baseHeader.sourceHops;
		m_result					= ResultId_Unknown;

		setState( State_ReadAddress );
		return ProcessResult_Ok;
	}

	RuntimeLow::ProcessResult RuntimeLow::processAddress( Reader& reader )
	{
		m_stateValue += reader.readData( getWorkingData(), m_destinationAddressSize, ( uintreg )m_stateValue );
		if( m_stateValue < m_destinationAddressSize )
		{
			return ProcessResult_WaitingData;
		}

		m_workingDataOffset += m_destinationAddressSize;

		if( m_playloadSize > getRemainingWorkingData() )
		{
			sendErrorResponse( ResultId_OutOfMemory );
			return ProcessResult_Error;
		}

		setState( State_ReadMessageHeader );
		return ProcessResult_Ok;
	}

	RuntimeLow::ProcessResult RuntimeLow::processMessageHeader( Reader& reader )
	{
		if( m_stateValue == 0u )
		{
			uint8 commandTypeValue;
			if( !reader.readByte( commandTypeValue ) )
			{
				return ProcessResult_WaitingData;
			}

			m_messageType = ( MessageType )commandTypeValue;
			m_stateValue++;
		}

		if( m_stateValue < sizeof( m_requestId ) + 1u )
		{
			m_stateValue += reader.readShort( m_requestId, uintreg( m_stateValue - 1u ) );
			if( m_stateValue < sizeof( m_requestId ) + 1u )
			{
				return ProcessResult_WaitingData;
			}
		}

		setState( State_ReadMessage );
		return ProcessResult_Ok;
	}

	RuntimeLow::ProcessResult RuntimeLow::processMessage( Reader& reader )
	{
		m_stateValue = reader.readData( getWorkingData(), ( uintreg )m_playloadSize, ( uintreg )m_stateValue );
		if( m_stateValue < m_playloadSize )
		{
			return ProcessResult_WaitingData;
		}

		m_workingDataOffset += m_playloadSize;

		setState( State_ExecuteCommand );
		return ProcessResult_Ok;
	}

	void RuntimeLow::executeCommand()
	{
		switch( m_messageType )
		{
		case MessageType_PingRequest:
			sendPingResponse();
			break;

		case MessageType_GetPropertyRequest:
			{
				Value value;
				{
					const GetPropertyRequest& request = *reinterpret_cast< const GetPropertyRequest* >( m_workingData + m_workingDataOffset );

					const LocalInstance* pInstance = findInstance( request.instanceId );
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

		//case MessageType_SetPropertyRequest:
		//	break;

		//case MessageType_CallFunctionRequest:
		//	break;

		default:
			{
				sendErrorResponse( ResultId_Unsupported );
				return;
			}
			break;
		}

		setState( State_ReadUntilNextMessage );
	}

	void RuntimeLow::sendPingResponse()
	{
		m_messageType = MessageType_PingResponse;
		setState( State_SendResponse );
	}

	void RuntimeLow::sendErrorResponse( ResultId result )
	{
		m_messageType	= MessageType_ErrorResponse;
		m_result		= result;
		setState( State_SendResponse );
	}

	void RuntimeLow::sendResponse( MessageType responseType, const void* pData, uint8 dataLength )
	{
		if( getRemainingWorkingData() < dataLength )
		{
			sendErrorResponse( ResultId_OutOfMemory );
			return;
		}

		copyMemory( getWorkingData(), pData, dataLength );

		m_messageType	= responseType;
		m_result		= ResultId_Success;

		setState( State_SendResponse, dataLength );
	}

	void RuntimeLow::sendData( Port* pPort )
	{
		// Send Base Header
		// Send Address 1
		// Send Address 2
		// Send Payload

		//Writer writer;
		//if( !pPort->openSend( writer, 0u ) )
		//{
		//	return;
		//}

		//const muint written = writer.writeData( getWorkingData(), (muint)m_stateValue );
		//m_stateValue -= written;
		//m_workingDataOffset += written;

		//pPort->closeSend( writer );

		//if( m_stateValue == 0u )
		//{
		//	setState( State_ReadUntilNextMessage );
		//}
	}
}