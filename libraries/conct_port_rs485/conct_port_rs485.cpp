#include "conct_port_rs485.h"

namespace conct
{
	void PortRs485::setup()
	{

	}

	void PortRs485::loop()
	{

	}

	bool PortRs485::openSend( Writer& writer, uintreg size, uintreg endpointId )
	{
		return false;
	}

	void PortRs485::closeSend( Writer& writer, uintreg endpointId )
	{

	}

	bool PortRs485::openReceived( Reader& reader, uintreg& endpointId )
	{
		return false;
	}

	void PortRs485::closeReceived( Reader& reader, uintreg endpointId )
	{

	}
	
	Flags8< PortFlag > PortRs485::getFlags()
	{
		Flags8< PortFlag > flags;
		flags |= PortFlag_SingleEndpoint;
		flags |= PortFlag_Reliable;
		return flags;
	}
}
