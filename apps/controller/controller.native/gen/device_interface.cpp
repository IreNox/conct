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
		m_runtime.registerPort( &m_port0 );

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
		 return "AndroidController";
	}

	void DeviceInterface::getEmptyInstances( Array< Instance >& instances )
	{
		static Instance s_instances[ 1u ];
		instances.set( s_instances, CONCT_COUNT( s_instances ) );
	}

	void DeviceInterface::getPublicInstances( ArrayView< Instance >& instances ) const
	{
		static const Instance s_instances[] =
		{
			{ 0, 32636 },
		};

		instances.set( s_instances, CONCT_COUNT( s_instances ) );
	}

	void DeviceInterface::getLocalInstances( ArrayView< LocalInstance >& instances )
	{
		static const LocalInstance s_instances[] =
		{
			{ 0, this, &m_proxyDevice },
		};

		instances.set( s_instances, CONCT_COUNT( s_instances ) );
	}
}
