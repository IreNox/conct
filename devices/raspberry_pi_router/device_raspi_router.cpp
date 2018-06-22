#include "device_raspi_router.h"

namespace conct
{
	void DeviceRaspiRouter::setup()
	{
		PortSerialConfig config;
		config.portName = "/dev/serial0"_s;

		m_port1.setConfig( config );
	}

	void DeviceRaspiRouter::loop()
	{
		// TODO: implement
	}
}
