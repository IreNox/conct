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

		m_ports.pushBack( pPort );
		m_runtime.registerPort( pPort );
		return pPort;
	}

	void DeviceController::removePort( PortTcpClient* pPort )
	{
		for( uintreg i = 0u; i < m_ports.getLength(); ++i )
		{
			if( m_ports[ i ] == pPort )
			{
				m_ports.eraseUnsortedByIndex( i );
				break;
			}
		}

		m_runtime.unregisterPort( pPort );
		delete pPort;
	}

	void DeviceController::setup()
	{
		m_controller.setup( &m_runtime );
	}

	void DeviceController::loop()
	{
		for( PortTcpClient* pPort : m_ports )
		{
			pPort->loop();
			m_runtime.processPort( pPort );
		}

		m_controller.loop();
	}
}
