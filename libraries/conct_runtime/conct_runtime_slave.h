#pragma once

#include "conct_core.h"
#include "conct_array.h"
#include "conct_reader.h"

namespace conct
{
	class Device;
	struct ProxyRegistry;

	class RuntimeSlave
	{
	public:

		void					setup( const Device* pDevice );

		void					processData( Reader& reader );

	private:

		enum MessageState : uint8_t
		{
			MessageState_None,
			MessageState_ReadBaseHeader,
			MessageState_ReadAddress,
			MessageState_ReadMessage
		};

		enum ProcessResult : uint8_t
		{
			ProcessResult_Ok,
			ProcessResult_WaitingData,
			ProcessResult_Error
		};

		MessageState			m_messageState;
		uint16_t				m_messagePlayloadSize;
		uint8_t*				m_pMessageDestinationAddress;
		uint8_t					m_messageDestinationAddressSize;
		uint8_t					m_messageWorkingDataOffset;
		uint8_t					m_messageWorkingData[ 64u ];

		Array< ProxyRegistry >	m_proxies;

		ProcessResult			processBaseHeader( Reader& reader );
		ProcessResult			processAddress( Reader& reader );
		ProcessResult			processMessage( Reader& reader );
	};
}
