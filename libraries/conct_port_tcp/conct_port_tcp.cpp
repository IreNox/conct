#include "conct_port_tcp.h"

namespace conct
{
	void PortTcp::setup()
	{

	}

	void PortTcp::loop()
	{

	}

	bool PortTcp::openSend( Writer& writer, uintreg size, DeviceId deviceId )
	{
		return false;
	}

	void PortTcp::closeSend( Writer& writer )
	{

	}

	bool PortTcp::openReceived( Reader& reader, DeviceId& deviceId )
	{
		return false;
	}

	void PortTcp::closeReceived( Reader& reader )
	{

	}

	Flags8< PortFlag > PortTcp::getFlags()
	{
		Flags8< PortFlag > flags;
		flags |= PortFlag_MultiEndpoint;
		flags |= PortFlag_Reliable;
		return flags;
	}
}
