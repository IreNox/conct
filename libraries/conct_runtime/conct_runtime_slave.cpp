#include "conct_runtime_slave.h"

#include "conct_runtime_types.h"

namespace conct
{
	void RuntimeSlave::setup( const Array< Proxy* >& proxies )
	{
		m_messageState				= MessageState_None;
		m_messagePlayloadSize		= 0u;
		m_messageWorkingDataOffset	= 0u;
		m_proxies = proxies;
	}

	void RuntimeSlave::processData( const Array< uint8_t >& data )
	{

	}

	void RuntimeSlave::processBaseHeader( Array< uint8_t >& data )
	{
		const RuntimeMessageBaseHeader& baseHeader = *( const RuntimeMessageBaseHeader* )data.getData();

		m_messagePlayloadSize			= baseHeader.payloadSize;
		m_messageDestinationAddressSize	= baseHeader.sourceHops;
		m_pMessageDestinationAddress	= m_messageWorkingData;
		m_messageWorkingDataOffset		= baseHeader.sourceHops;

		m_messageState = MessageState_ReadAddress;

		data = 
	}

	void RuntimeSlave::processAddress( Array< uint8_t >& data )
	{

	}

	void RuntimeSlave::processMessage( Array< uint8_t >& data )
	{

	}

}