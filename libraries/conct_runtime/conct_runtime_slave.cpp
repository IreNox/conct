#include "conct_runtime_slave.h"

#include "conct_device.h"
#include "conct_runtime_types.h"

namespace conct
{
	void RuntimeSlave::setup( const Device* pDevice )
	{
		pDevice->getProxies( m_proxies );

		m_messageState				= MessageState_None;
		m_messagePlayloadSize		= 0u;
		m_messageWorkingDataOffset	= 0u;
	}

	void RuntimeSlave::processData( Reader& reader )
	{
		bool result = true;
		while(  result && !reader.isEnd() )
		{
			switch( m_messageState )
			{
			case MessageState_ReadBaseHeader:
				result = processBaseHeader( reader );
				break;

			case MessageState_ReadAddress:
				result = processAddress( reader );
				break;

			case MessageState_ReadMessage:
				result = processMessage( reader );
				break;
			}
		}
	}

	bool RuntimeSlave::processBaseHeader( Reader& reader )
	{
		const RuntimeMessageBaseHeader* pBaseHeader = reader.readStruct< RuntimeMessageBaseHeader >();
		if( pBaseHeader == nullptr )
		{
			return false;
		}

		m_messagePlayloadSize			= pBaseHeader->payloadSize;
		m_messageDestinationAddressSize	= pBaseHeader->sourceHops;
		m_messageWorkingDataOffset		= 0u;

		m_messageState = MessageState_ReadAddress;
		return true;
	}

	bool RuntimeSlave::processAddress( Reader& reader )
	{
		if( !reader.readData( m_messageWorkingData, m_messageDestinationAddressSize ) )
		{
			return false;
		}

		m_pMessageDestinationAddress	= m_messageWorkingData;
		m_messageWorkingDataOffset		= m_messageDestinationAddressSize;

		m_messageState = MessageState_ReadMessage;
		return true;
	}

	bool RuntimeSlave::processMessage( Reader& reader )
	{

	}

}