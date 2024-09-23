#include "device_interface.h"

namespace conct
{
	DeviceInterface::DeviceInterface()
	{
	}

	void DeviceInterface::setupDevice()
	{
		PortTcpClientParameters port0Parameters;
		port0Parameters.serverHost = "::1"_s;
		port0Parameters.serverPort = 5489;
		m_port0.setup( port0Parameters );

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
		 return "ControllerSimulator";
	}

	void DeviceInterface::getEmptyInstances( ArrayView< Instance >& instances )
	{
		static Instance s_instances[ 1u ];
		instances = ArrayView< Instance >( s_instances, TIKI_ARRAY_COUNT( s_instances ) );
	}

	void DeviceInterface::getPublicInstances( ConstInstanceView& instances ) const
	{
		static const Instance s_instances[] =
		{
			{ 0, 32636 },
		};

		instances.set( s_instances, TIKI_ARRAY_COUNT( s_instances ) );
	}

	void DeviceInterface::getLocalInstances( ArrayView< const LocalInstance >& instances )
	{
		static const LocalInstance s_instances[] =
		{
			{ 0, this, &m_proxyDevice },
		};

		instances.set( s_instances, TIKI_ARRAY_COUNT( s_instances ) );
	}
}
