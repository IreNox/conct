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
#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
			State_ReadCryptoHeader,
#endif
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

		Device*						m_pDevice;

		State						m_state;
		uintreg						m_stateValue;

		uint8						m_workingData[ 64u ];
		uintreg						m_workingDataOffset;

		uint16						m_playloadSize;
		uint8						m_destinationAddressSize;
		MessageType					m_messageType;
		CommandId					m_commandId;
		ResultId					m_result;

		void*						getWorkingData();
		uintreg						getRemainingWorkingData() const;

		void						setState( State state, uintreg stateValue = 0u );

		void						readData( Port* pPort );
		void						readData( Reader& reader );

#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		ReadResult					readCryptoHeader( Reader& reader );
#endif
		ReadResult					readBaseHeader( Reader& reader );
		ReadResult					readAddresses( Reader& reader );
		ReadResult					readPayload( Reader& reader );

		void						executeCommand();

		void						sendPingResponse();
		void						sendErrorResponse( MessageType responseType, ResultId result );
		void						sendResponse( MessageType responseType, const void* pData, uintreg dataLength );

		void						sendData( Port* pPort );
	};
}
