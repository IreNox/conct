#include "device_interface.h"

namespace conct
{
	void DeviceInterface::setupDevice()
	{
		m_port0.setup();

		m_runtime.setup( this );

		setup();
	}

	void DeviceInterface::loopDevice()
	{
		m_port0.loop();

		m_runtime.processPort( &m_port0 );

		loop();
	}

	void DeviceInterface::getInstances( ArrayView< LocalInstance >& instances )
	{
		static const LocalInstance s_instances[] =
		{
			{ 0, &m_instanceLight, &m_proxyLight },
			{ 1, &m_instanceDimmer, &m_proxyDimmer },
			{ 2, &m_instanceRGB, &m_proxyRGB },
		};

		instances.set( s_instances, CONCT_COUNT( s_instances ) );
	}
}
