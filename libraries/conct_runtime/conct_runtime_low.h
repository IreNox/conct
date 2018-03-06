#pragma once

#include "conct_array_view.h"
#include "conct_core.h"
#include "conct_runtime_types.h"

namespace conct
{
	class Device;
	class Port;
	class Reader;

	class RuntimeLow
	{
	public:

		void					setup( Device* pDevice );

		void					processPort( Port* pPort );

	private:

		enum State : uint8_t
		{
			State_ReadBaseHeader,
			State_ReadAddress,
			State_ReadMessage,
			State_ReadUntilNextMessage,
			State_ExecuteCommand,
			State_SendResponse
		};

		enum ProcessResult : uint8_t
		{
			ProcessResult_Ok,
			ProcessResult_WaitingData,
			ProcessResult_Error
		};

		State						m_state;
		uint16_t					m_stateValue;

		uint8_t						m_workingData[ 64u ];
		uint8_t						m_workingDataOffset;

		uint16_t					m_playloadSize;
		uint8_t						m_destinationAddressSize;
		RuntimeMessageType			m_messageType;
		RuntimeRequestId			m_requestId;

		ArrayView< LocalInstance >	m_instances;

		uint8_t						getRemainingWorkingData() const;

		const LocalInstance*		findInstance( InstanceId instaceId );

		void						setState( State state, uint16_t statValue );

		void						processData( Port* pPort );
		void						processData( Reader& reader );
		ProcessResult				processBaseHeader( Reader& reader );
		ProcessResult				processAddress( Reader& reader );
		ProcessResult				processMessage( Reader& reader );
		ProcessResult				processUntilNextMessage( Reader& reader );

		void						sendPingResponse();
		void						sendErrorResponse( RuntimeErrorCode error );
		void						sendResponse( RuntimeMessageType responseType, const void* pData, uint8_t dataLength );

		void						sendData( Port* pPort );
	};
}
