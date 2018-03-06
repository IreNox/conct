#include "conct_runtime_low.h"

#include "conct_device.h"
#include "conct_port.h"
#include "conct_proxy.h"
#include "conct_reader.h"
#include "conct_runtime_types.h"
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
		if( m_state == State_SendResponse )
		{
			sendData( pPort );
		}
		else
		{
			processData( pPort );
		}
	}

	uint8_t RuntimeLow::getRemainingWorkingData() const
	{
		return sizeof( m_workingData ) - m_workingDataOffset;
	}

	const LocalInstance* RuntimeLow::findInstance( InstanceId instaceId )
	{
		for( uint8_t i = 0u; i < m_instances.getCount(); ++i )
		{
			if( m_instances[ i ].id == instaceId )
			{
				return &m_instances[ i ];
			}
		}

		return nullptr;
	}

	void RuntimeLow::setState( State state, uint16_t statValue )
	{
		m_state			= state;
		m_stateValue	= statValue;
	}

	void RuntimeLow::processData( Port* pPort )
	{
		Reader reader;
		if( !pPort->openReceived( reader, 0u ) )
		{
			return;
		}

		processData( reader );

		pPort->closeReceived();
	}

	void RuntimeLow::processData( Reader& reader )
	{
		ProcessResult result = ProcessResult_Ok;
		while( !reader.isEnd() )
		{
			switch( m_state )
			{
			case State_ReadBaseHeader:
				result = processBaseHeader( reader );
				break;

			case State_ReadAddress:
				result = processAddress( reader );
				break;

			case State_ReadMessage:
				result = processMessage( reader );
				break;

			case State_ReadUntilNextMessage:
				result = processUntilNextMessage( reader );
				break;
			}

			if( result == ProcessResult_Error )
			{
				setState( State_ReadUntilNextMessage, 0u );
			}
			else if( result == ProcessResult_WaitingData )
			{
				CONCT_ASSERT( reader.isEnd() );
				break;
			}
		}
	}

	RuntimeLow::ProcessResult RuntimeLow::processBaseHeader( Reader& reader )
	{
		const RuntimeMessageBaseHeader* pBaseHeader = reader.readStruct< RuntimeMessageBaseHeader >();
		if( pBaseHeader == nullptr )
		{
			return ProcessResult_Error;
		}

		if( pBaseHeader->destinationHops > 0u )
		{
			return ProcessResult_Error;
		}

		if( pBaseHeader->sourceHops > sizeof( m_workingData ) )
		{
			return ProcessResult_Error;
		}

		m_playloadSize				= pBaseHeader->payloadSize;
		m_destinationAddressSize	= pBaseHeader->sourceHops;

		m_workingDataOffset			= 0u;

		setState( State_ReadAddress, m_destinationAddressSize );
		return ProcessResult_Ok;
	}

	RuntimeLow::ProcessResult RuntimeLow::processAddress( Reader& reader )
	{
		const uint8_t remainingSize = ( uint8_t )CONCT_MIN( m_stateValue, 255u );
		const uint8_t readSize = reader.readData( m_workingData + m_workingDataOffset, remainingSize );
		m_stateValue -= readSize;
		m_workingDataOffset += readSize;
		if( m_stateValue > 0u )
		{
			return ProcessResult_WaitingData;
		}

		setState( State_ReadMessage, 0u );
		return ProcessResult_Ok;
	}

	RuntimeLow::ProcessResult RuntimeLow::processMessage( Reader& reader )
	{
		if( m_stateValue == 0u )
		{
			if( m_playloadSize > getRemainingWorkingData() )
			{
				return ProcessResult_Error;
			}

			uint8_t commandTypeValue;
			if( !reader.readByte( commandTypeValue ) )
			{
				return ProcessResult_WaitingData;
			}
			m_messageType = ( RuntimeMessageType )commandTypeValue;
			m_stateValue++;
		}

		if( m_stateValue < sizeof( m_requestId ) + 1u )
		{
			m_stateValue += reader.readShort( m_requestId, ( uint8_t )( ( sizeof( m_requestId ) + 1u ) - m_stateValue ) );
			if( m_stateValue < sizeof( m_requestId ) + 1u )
			{
				return ProcessResult_WaitingData;
			}
		}

		if( m_stateValue < m_playloadSize )
		{
			const uint8_t remainingSize = ( uint8_t )( m_playloadSize - m_stateValue );
			const uint8_t readSize = reader.readData( m_workingData, (uint8_t)m_playloadSize, remainingSize );
			m_stateValue += readSize;

			if( m_stateValue < m_playloadSize )
			{
				return ProcessResult_WaitingData;
			}
		}
		CONCT_ASSERT( reader.isEnd() );
		CONCT_ASSERT( m_stateValue == m_playloadSize );

		switch( m_messageType )
		{
		case RuntimeMessageType_PingRequest:
			sendPingResponse();
			break;

		case RuntimeMessageType_GetPropertyRequest:
			{
				Value value;
				{
					const RuntimeGetPropertyRequest& request = *reinterpret_cast< const RuntimeGetPropertyRequest* >( m_workingData + m_workingDataOffset );

					const LocalInstance* pInstance = findInstance( request.instanceId );
					if( pInstance == nullptr )
					{
						sendErrorResponse( RuntimeErrorCode_NoSuchInstance );
						return ProcessResult_Ok;
					}

					if( !pInstance->pProxy->getProperty( value, pInstance->pInstance, request.name ) )
					{
						sendErrorResponse( RuntimeErrorCode_NoSuchField );
						return ProcessResult_Ok;
					}
				}

				sendResponse( RuntimeMessageType_GetPropertyResponse, &value, sizeof( value ) );
			}
			break;

		case RuntimeMessageType_SetPropertyRequest:
			break;

		case RuntimeMessageType_CallFunctionRequest:
			break;

		default:
			return ProcessResult_Error;
			break;
		}

		return ProcessResult_Ok;
	}

	RuntimeLow::ProcessResult RuntimeLow::processUntilNextMessage( Reader& reader )
	{
		m_stateValue += reader.getRemainingSize();

		if( m_stateValue < m_playloadSize )
		{
			return ProcessResult_WaitingData;
		}

		m_state = State_ReadBaseHeader;
		return ProcessResult_Ok;
	}

	void RuntimeLow::sendPingResponse()
	{
		m_messageType = RuntimeMessageType_PingResponse;
		setState( State_SendResponse, 0u );
	}

	void RuntimeLow::sendErrorResponse( RuntimeErrorCode error )
	{
		m_messageType = RuntimeMessageType_ErrorResponse;
		setState( State_SendResponse, error );
	}

	void RuntimeLow::sendResponse( RuntimeMessageType responseType, const void* pData, uint8_t dataLength )
	{
		if( getRemainingWorkingData() < dataLength )
		{
			sendErrorResponse( RuntimeErrorCode_OutOfMemory );
			return;
		}

		m_messageType = responseType;
		copyMemory( m_workingData + m_workingDataOffset, pData, dataLength );
		setState( State_SendResponse, dataLength );
	}

	void RuntimeLow::sendData( Port* pPort )
	{
		Writer writer;
		if( !pPort->openSend( writer, 0u ) )
		{
			return;
		}



		pPort->closeSend();
	}
}