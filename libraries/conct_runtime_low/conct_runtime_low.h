#pragma once

#include "conct_array_view.h"
#include "conct_core.h"
#include "conct_runtime.h"

namespace conct
{
	class Device;
	class Port;
	class Reader;
	class Writer;

	class RuntimeLow
	{
	public:

		void					setup( Device* pDevice );

		void					processPort( Port* pPort );

	private:

		enum State : uint8
		{
			//State_ReadUntilNextMessage,
			State_ReadBaseHeader,
			State_ReadAddresses,
			State_ReadPayload,
			State_ExecuteCommand,
			State_Send,
		};

		enum ReadResult : uint8
		{
			ReadResult_Ok,
			ReadResult_WaitingData,
			ReadResult_Error
		};

		State						m_state;
		uint16						m_stateValue;

		uint8						m_workingData[ 64u ];
		uint8						m_workingDataOffset;

		uint16						m_playloadSize;
		uint8						m_destinationAddressSize;
		MessageType					m_messageType;
		CommandId					m_commandId;
		ResultId					m_result;

		ArrayView< LocalInstance >	m_instances;

		void*						getWorkingData();
		uint8						getRemainingWorkingData() const;

		const LocalInstance*		findInstanceById( InstanceId instaceId );
		const LocalInstance*		findInstanceByType( TypeCrc typeCrc );

		void						setState( State state, uint16 stateValue = 0u );

		void						readData( Port* pPort );
		void						readData( Reader& reader );
		//ReadResult					readUntilNextMessage( Reader& reader );
		ReadResult					readBaseHeader( Reader& reader );
		ReadResult					readAddresses( Reader& reader );
		ReadResult					readPayload( Reader& reader );

		void						executeCommand();

		void						sendPingResponse();
		void						sendErrorResponse( ResultId result );
		void						sendResponse( MessageType responseType, const void* pData, uint8 dataLength );

		void						sendData( Port* pPort );
	};
}
