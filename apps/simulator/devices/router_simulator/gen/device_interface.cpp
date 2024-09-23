#include "device_interface.h"

namespace conct
{
	DeviceInterface::DeviceInterface()
		: m_instanceRouter( m_runtime )
	{
	}

	void DeviceInterface::setupDevice()
	{
		PortTcpServerParameters port0Parameters;
		port0Parameters.listenPort = 5489;
		port0Parameters.listenAddress = "::0"_s;
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
		 return "RouterSimulator";
	}

	void DeviceInterface::getEmptyInstances( ArrayView< Instance >& instances )
	{
		static Instance s_instances[ 4u ];
		instances = ArrayView< Instance >( s_instances, TIKI_ARRAY_COUNT( s_instances ) );
	}

	void DeviceInterface::getPublicInstances( ConstInstanceView& instances ) const
	{
		static const Instance s_instances[] =
		{
			{ 0, 32636 },
			{ 1, 25653 },
			{ 2, 19862 },
			{ 3, 26478 },
		};

		instances.set( s_instances, TIKI_ARRAY_COUNT( s_instances ) );
	}

	void DeviceInterface::getLocalInstances( ArrayView< const LocalInstance >& instances )
	{
		static const LocalInstance s_instances[] =
		{
			{ 0, this, &m_proxyDevice },
			{ 1, &m_instanceRouter, &m_proxyRouter },
			{ 2, &m_instanceBrowser, &m_proxyBrowser },
			{ 3, &m_instanceClipboard, &m_proxyClipboard },
		};

		instances.set( s_instances, TIKI_ARRAY_COUNT( s_instances ) );
	}
}
