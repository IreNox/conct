#include "conct_port_lan.h"

namespace conct
{
	void PortLan::setup()
	{

	}

	void PortLan::loop()
	{

	}

	bool PortLan::openSend( Writer& writer, uintreg size, DeviceId deviceId )
	{
		return false;
	}

	void PortLan::closeSend( Writer& writer )
	{

	}

	bool PortLan::openReceived( Reader& reader, DeviceId& deviceId )
	{
		return false;
	}

	void PortLan::closeReceived( Reader& reader )
	{

	}

	Flags8< PortFlag > PortLan::getFlags()
	{
		Flags8< PortFlag > flags;
		flags |= PortFlag_MultiEndpoint;
		flags |= PortFlag_Reliable;
		return flags;
	}
}
