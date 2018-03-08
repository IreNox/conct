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

	void RuntimeLow::setState( State state, uint16_t stateValue /* = 0u */ )
	{
		m_state			= state;
		m_stateValue	= stateValue;
	}

	void RuntimeLow::processData( Port* pPort )
	{
		Reader reader;
		if( !pPort->openReceived( reader, 0u ) )
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
		uint16_t& magic = *( uint16_t* )m_workingData;

		if( m_stateValue < 2u )
		{
			m_stateValue += reader.readShort( magic, muint( sizeof( s_magic ) - m_stateValue ) );

			if( m_stateValue <= sizeof( s_magic ) )
			{
				return ProcessResult_WaitingData;
			}

			if( magic == s_magic )
			{
				setState( State_ReadBaseHeader );
				return ProcessResult_Ok;
			}
		}

		uint8_t nextByte = 0u;
		while( reader.readByte( nextByte ) )
		{
			magic <<= 8u;
			magic |= nextByte;

			if( magic == s_magic )
			{
				setState( State_ReadBaseHeader );
				return ProcessResult_Ok;
			}
		}

		return ProcessResult_WaitingData;
	}

	RuntimeLow::ProcessResult RuntimeLow::processBaseHeader( Reader& reader )
	{
		RuntimeMessageBaseHeader& baseHeader = *static_cast< RuntimeMessageBaseHeader* >( getWorkingData() );
		m_stateValue += reader.readStruct< RuntimeMessageBaseHeader >( baseHeader, ( muint )m_stateValue );
		if( m_stateValue < sizeof( baseHeader ) )
		{
			return ProcessResult_WaitingData;
		}

		if( baseHeader.destinationHops > 0u )
		{
			sendErrorResponse( RuntimeResult_Unsupported );
			return ProcessResult_Error;
		}

		if( baseHeader.sourceHops > sizeof( m_workingData ) )
		{
			sendErrorResponse( RuntimeResult_OutOfMemory );
			return ProcessResult_Error;
		}

		m_workingDataOffset			= 0u;
		m_playloadSize				= baseHeader.payloadSize;
		m_destinationAddressSize	= baseHeader.sourceHops;
		m_result					= RuntimeResult_Unknown;

		setState( State_ReadAddress );
		return ProcessResult_Ok;
	}

	RuntimeLow::ProcessResult RuntimeLow::processAddress( Reader& reader )
	{
		m_stateValue += reader.readData( getWorkingData(), m_destinationAddressSize, ( muint )m_stateValue );
		if( m_stateValue < m_destinationAddressSize )
		{
			return ProcessResult_WaitingData;
		}

		m_workingDataOffset += m_destinationAddressSize;

		if( m_playloadSize > getRemainingWorkingData() )
		{
			sendErrorResponse( RuntimeResult_OutOfMemory );
			return ProcessResult_Error;
		}

		setState( State_ReadMessageHeader );
		return ProcessResult_Ok;
	}

	RuntimeLow::ProcessResult RuntimeLow::processMessageHeader( Reader& reader )
	{
		if( m_stateValue == 0u )
		{
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
			m_stateValue += reader.readShort( m_requestId, muint( m_stateValue - 1u ) );
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
		m_stateValue = reader.readData( getWorkingData(), ( muint )m_playloadSize, ( muint )m_stateValue );
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
						sendErrorResponse( RuntimeResult_NoSuchInstance );
						return;
					}

					if( !pInstance->pProxy->getProperty( value, pInstance->pInstance, request.name ) )
					{
						sendErrorResponse( RuntimeResult_NoSuchField );
						return;
					}
				}

				sendResponse( RuntimeMessageType_GetPropertyResponse, &value, sizeof( value ) );
			}
			break;

		//case RuntimeMessageType_SetPropertyRequest:
		//	break;

		//case RuntimeMessageType_CallFunctionRequest:
		//	break;

		default:
			{
				sendErrorResponse( RuntimeResult_Unsupported );
				return;
			}
			break;
		}

		setState( State_ReadUntilNextMessage );
	}

	void RuntimeLow::sendPingResponse()
	{
		m_messageType = RuntimeMessageType_PingResponse;
		setState( State_SendResponse );
	}

	void RuntimeLow::sendErrorResponse( RuntimeResult result )
	{
		m_messageType	= RuntimeMessageType_ErrorResponse;
		m_result		= result;
		setState( State_SendResponse );
	}

	void RuntimeLow::sendResponse( RuntimeMessageType responseType, const void* pData, uint8_t dataLength )
	{
		if( getRemainingWorkingData() < dataLength )
		{
			sendErrorResponse( RuntimeResult_OutOfMemory );
			return;
		}

		copyMemory( getWorkingData(), pData, dataLength );

		m_messageType	= responseType;
		m_result		= RuntimeResult_Success;

		setState( State_SendResponse, dataLength );
	}

	void RuntimeLow::sendData( Port* pPort )
	{
		Writer writer;
		if( !pPort->openSend( writer, 0u ) )
		{
			return;
		}

		const muint written = writer.writeData( getWorkingData(), (muint)m_stateValue );
		m_stateValue -= written;
		m_workingDataOffset += written;

		pPort->closeSend( writer );

		if( m_stateValue == 0u )
		{
			setState( State_ReadUntilNextMessage );
		}
	}
}