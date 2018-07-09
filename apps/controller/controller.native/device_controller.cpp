#include "device_controller.h"

#include "conct_port_tcp_client.h"

namespace conct
{
	PortTcpClient* DeviceController::addPort( const char* pHostname, uint16 hostPort )
	{
		PortTcpClient* pPort = new PortTcpClient();

		PortTcpClientConfig config;
		config.targetHost	= DynamicString( pHostname );
		config.targetPort	= hostPort;
		pPort->setConfig( config );
		if( !pPort->setup() )
		{
			delete pPort;
			return nullptr;
		}

		m_runtime.registerPort( pPort );
		return pPort;
	}

	void DeviceController::removePort( PortTcpClient* pPort )
	{
		m_runtime.unregisterPort( pPort );
		delete pPort;
	}

	void DeviceController::setup()
	{
		m_controller.setup( &m_runtime );
	}

	void DeviceController::loop()
	{
		m_controller.loop();
	}
}
