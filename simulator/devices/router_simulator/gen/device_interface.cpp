#include "device_interface.h"

namespace conct
{
	void DeviceInterface::setupDevice()
	{
		m_port0.setup();
		m_port1.setup();

		m_runtime.setup( this );
		m_runtime.registerPort( &m_port0 );
		m_runtime.registerPort( &m_port1 );

		setup();
	}

	void DeviceInterface::loopDevice()
	{
		m_port0.loop();
		m_port1.loop();

		m_runtime.processPort( &m_port0 );
		m_runtime.processPort( &m_port1 );

		loop();
	}

	void DeviceInterface::getInstances( ArrayView< LocalInstance >& instances )
	{
		instances.set( nullptr, 0u );
	}
}
