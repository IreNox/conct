#include "device_android_controller.h"

#include "conct_port_tcp_client.h"

namespace conct
{
	PortTcpClient* DeviceAndroidController::addPort( const char* pHostname, uint16 hostPort )
	{
		PortTcpClient* pPort = new PortTcpClient();

		PortTcpClientConfig config;
		config.targetHost	= DynamicString( pHostname );
		config.targetPort	= hostPort;
		pPort->setConfig( config );
		pPort->setup();

		m_runtime.registerPort( pPort );
		return pPort;
	}

	void DeviceAndroidController::removePort( PortTcpClient* pPort )
	{
		m_runtime.unregisterPort( pPort );
		delete pPort;
	}

	void DeviceAndroidController::setup()
	{
		m_controller.setup( &m_runtime );
	}

	void DeviceAndroidController::loop()
	{
		m_controller.loop();
	}
}
