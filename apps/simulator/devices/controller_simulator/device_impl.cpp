#include "device_impl.h"

namespace conct
{
	void DeviceControllerSimulator::setup()
	{
		PortTcpClientConfig config;
		config.targetHost	= "::1"_s; // "2a02:810c:740:3f0:e:8ec0:f40e:8b10"_s; // "raspberrypi"_s;
		config.targetPort	= 5489;

		m_port0.setConfig( config );
	}

	void DeviceControllerSimulator::loop()
	{
		// TODO: implement
	}
}
