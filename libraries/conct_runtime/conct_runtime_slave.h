#pragma once

#include "conct_core.h"
#include "conct_array.h"

namespace conct
{
	class Proxy;

	class RuntimeSlave
	{
	public:

		void	setup( const Array< Proxy* >& proxies );

		void	processData( const Array< uint8_t >& data );

	private:

		enum MessageState
		{
			MessageState_None,
			MessageState_ReadBaseHeader,
			MessageState_ReadAddress,
			MessageState_ReadMessage
		};

		MessageState	m_messageState;
		uint16_t		m_messagePlayloadSize;
		uint8_t*		m_pMessageDestinationAddress;
		uint8_t			m_messageDestinationAddressSize;
		uint8_t			m_messageWorkingDataOffset;
		uint8_t			m_messageWorkingData[ 64u ];

		Array< Proxy* >	m_proxies;

		void			processBaseHeader( Array< uint8_t >& data );
		void			processAddress( Array< uint8_t >& data );
		void			processMessage( Array< uint8_t >& data );
	};
}
