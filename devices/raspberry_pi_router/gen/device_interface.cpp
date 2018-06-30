#include "device_interface.h"

namespace conct
{
	DeviceInterface::DeviceInterface()
		: m_instanceRouter( m_runtime )
	{
	}

	void DeviceInterface::setupDevice()
	{
		setup();

		m_port0.setup();
		m_port1.setup();

		m_runtime.setup( this );
		m_runtime.registerPort( &m_port0 );
		m_runtime.registerPort( &m_port1 );
	}

	void DeviceInterface::loopDevice()
	{
		m_port0.loop();
		m_port1.loop();

		m_runtime.processPort( &m_port0 );
		m_runtime.processPort( &m_port1 );

		loop();
	}

	const char* DeviceInterface::getName() const
	{
		 return "RaspiRouter";
	}

	void DeviceInterface::getEmptyInstances( Array< Instance >& instances )
	{
		static Instance s_instances[ 2u ];
		instances.set( s_instances, CONCT_COUNT( s_instances ) );
	}

	void DeviceInterface::getPublicInstances( ArrayView< Instance >& instances ) const
	{
		static const Instance s_instances[] =
		{
			{ 0, 32636 },
			{ 1, 25653 },
		};

		instances.set( s_instances, CONCT_COUNT( s_instances ) );
	}

	void DeviceInterface::getLocalInstances( ArrayView< LocalInstance >& instances )
	{
		static const LocalInstance s_instances[] =
		{
			{ 0, this, &m_proxyDevice },
			{ 1, &m_instanceRouter, &m_proxyRouter },
		};

		instances.set( s_instances, CONCT_COUNT( s_instances ) );
	}
}