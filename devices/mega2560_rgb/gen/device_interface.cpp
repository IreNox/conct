#include "device_interface.h"

namespace conct
{
	DeviceInterface::DeviceInterface()
	{
	}

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

	const char* DeviceInterface::getName() const
	{
		 return "mega2560_rgb";
	}

	void DeviceInterface::getEmptyInstances( Array< Instance >& instances )
	{
		static Instance s_instances[ 4u ];
		instances.set( s_instances, CONCT_COUNT( s_instances ) );
	}

	void DeviceInterface::getPublicInstances( ArrayView< Instance >& instances ) const
	{
		static const Instance s_instances[] =
		{
			{ 0, 32636 },
			{ 1, 24332 },
			{ 2, 57793 },
			{ 3, 7621 },
		};

		instances.set( s_instances, CONCT_COUNT( s_instances ) );
	}

	void DeviceInterface::getLocalInstances( ArrayView< LocalInstance >& instances )
	{
		static const LocalInstance s_instances[] =
		{
			{ 0, this, &m_proxyDevice },
			{ 1, &m_instanceLight, &m_proxyLight },
			{ 2, &m_instanceDimmer, &m_proxyDimmer },
			{ 3, &m_instanceRGB, &m_proxyRGB },
		};

		instances.set( s_instances, CONCT_COUNT( s_instances ) );
	}
}
