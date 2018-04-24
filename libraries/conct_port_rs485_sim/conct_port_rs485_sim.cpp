#include "conct_port_rs485_sim.h"

#include "conct_reader.h"
#include "conct_writer.h"

namespace conct
{
	void PortRs485Sim::setup()
	{
		m_hasSent = false;
		m_hasReceived = false;
	}

	void PortRs485Sim::loop()
	{
		m_hasSent = false;
		m_hasReceived = true;
	}

	bool PortRs485Sim::openSend( Writer& writer, uintreg size, DeviceId deviceId )
	{
		if( m_hasReceived || m_hasSent )
		{
			return false;
		}

		writer.set( m_buffer, sizeof( m_buffer ) );
		return true;
	}

	void PortRs485Sim::closeSend( Writer& writer )
	{
		m_hasSent = true;
	}

	bool PortRs485Sim::openReceived( Reader& reader, DeviceId& deviceId )
	{
		if( m_hasReceived )
		{
			reader.set( m_buffer, sizeof( m_buffer ) );
			deviceId = 0u;
			return true;
		}

		return false;
	}

	void PortRs485Sim::closeReceived( Reader& reader )
	{
		m_hasReceived = false;
	}

	Flags8< PortFlag > PortRs485Sim::getFlags()
	{
		Flags8< PortFlag > flags;
		flags |= PortFlag_SingleEndpoint;
		flags |= PortFlag_Reliable;
		return flags;
	}
}
