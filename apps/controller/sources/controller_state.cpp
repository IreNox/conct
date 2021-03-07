#include "controller_state.h"

namespace conct
{
	void ControllerState::setup( ControllerConfig& config, RuntimeHigh& runtime )
	{
		m_pRuntime	= &runtime;
		m_pConfig	= &config;

		m_controller.setup( runtime );
	}

	void ControllerState::destroy()
	{
		for( Connection* pConnection : m_connections )
		{
			destroyConnection( pConnection );
		}
		m_connections.clear();

		for( ConnectedDevice* pDevice : m_devices )
		{
			destroyDevice( pDevice );
		}
		m_devices.clear();
	}

	void ControllerState::loop()
	{
		updateConfig();

		for( Connection* pConnection : m_connections )
		{
			pConnection->pClientPort->loop();
			m_pRuntime->processPort( pConnection->pClientPort );
		}

		m_controller.loop();

		updateDevices();
	}

	void ControllerState::updateConfig()
	{
		if( m_pConfig->getRevision() == m_configRevision )
		{
			return;
		}

		ConnectionVector lostConnections = m_connections;
		m_connections.clear();

		const ControllerConfig::ConnectionVector& connections = m_pConfig->getConnections();
		for( const ControllerConfig::Connection& configConnection : connections )
		{
			Connection* pConnection = nullptr;
			for( size_t i = 0u; i < lostConnections.getLength(); ++i )
			{
				Connection* pCurrentConnection = lostConnections[ i ];
				if( pCurrentConnection->hostname != configConnection.hostname ||
					pCurrentConnection->port != configConnection.port )
				{
					continue;
				}

				pConnection = pCurrentConnection;
				lostConnections.eraseUnsortedByIndex( i );
				break;
			}

			if( pConnection == nullptr )
			{
				pConnection = createConnection( configConnection.hostname, configConnection.port );
			}

			m_connections.pushBack( pConnection );
		}

		for( Connection* pLostConnection : lostConnections )
		{
			destroyConnection( pLostConnection );
		}

		m_configRevision = m_pConfig->getRevision();
	}

	void ControllerState::updateDevices()
	{
		DeviceVector lostDevices = m_devices;
		m_devices.clear();

		Vector< DeviceConnection > devices;
		m_pRuntime->getDevices( devices );

		DeviceAddress address;
		for( const DeviceConnection& connection : devices )
		{
			address.address[ 0u ] = InvalidDeviceId;
			runtime::pushToDeviceAddress( address, connection.id );

			ConnectedDevice* pDevice = nullptr;
			for( size_t i = 0u; i < lostDevices.getLength(); ++i )
			{
				ConnectedDevice* pCurrentDevice = lostDevices[ i ];
				if( !runtime::isDeviceAddressEquals( address, pCurrentDevice->address ) )
				{
					continue;
				}

				pDevice = pCurrentDevice;
				lostDevices.eraseUnsortedByIndex( i );
				break;
			}

			if( pDevice == nullptr )
			{
				pDevice = createDevice( address );
			}

			m_devices.pushBack( pDevice );
		}

		for( ConnectedDevice* pLostDevice : lostDevices )
		{
			destroyDevice( pLostDevice );
		}
	}

	ControllerState::Connection* ControllerState::createConnection( const DynamicString& hostname, uint16 port )
	{
		Connection* pConnection = new Connection();
		
		pConnection->hostname	= hostname;
		pConnection->port		= port;

		PortTcpClientParameters parameters;
		parameters.serverHost	= pConnection->hostname;
		parameters.serverPort	= pConnection->port;

		pConnection->pClientPort = new PortTcpClient();
		pConnection->pClientPort->setup( parameters );

		m_pRuntime->registerPort( pConnection->pClientPort );

		return pConnection;
	}

	void ControllerState::destroyConnection( Connection* pConnection )
	{
		m_pRuntime->unregisterPort( pConnection->pClientPort );
		delete pConnection->pClientPort;
		pConnection->pClientPort = nullptr;

		delete pConnection;
	}

	ControllerState::ConnectedDevice* ControllerState::createDevice( const DeviceAddress& address )
	{
		ConnectedDevice* pDevice = new ConnectedDevice();
		pDevice->address = address;

		for( size_t i = 0u; i < DeviceAddress::Size; ++i )
		{
			if( address.address[ i ] == InvalidDeviceId )
			{
				break;
			}

			if( !pDevice->name.isEmpty() )
			{
				pDevice->name += "-";
			}

			pDevice->name += string_tools::toHexString( address.address[ i ] );
		}

		RemoteInstance remoteDeviceInstance;
		remoteDeviceInstance.address	= address;
		remoteDeviceInstance.id			= 0u;

		pDevice->pNameCommand		= m_controller.getProperty( remoteDeviceInstance, "Name" );
		pDevice->pInstancesCommand	= m_controller.getProperty( remoteDeviceInstance, "Instances" );

		return pDevice;
	}

	void ControllerState::destroyDevice( ConnectedDevice* pDevice )
	{
		if( pDevice->pNameCommand != nullptr )
		{
			m_controller.releaseCommand( pDevice->pNameCommand );
		}

		if( pDevice->pInstancesCommand != nullptr )
		{
			m_controller.releaseCommand( pDevice->pInstancesCommand );
		}

		if( pDevice->pDevicesCommand != nullptr )
		{
			m_controller.releaseCommand( pDevice->pDevicesCommand );
		}

		for( DeviceInstance& instance : pDevice->instances )
		{
			destroyInstance( &instance );
		}

		delete pDevice;
	}

	void ControllerState::destroyInstance( DeviceInstance* pInstance )
	{
		for( InstanceProperty& property : pInstance->properties )
		{
			destroyProperty( &property );
		}

		pInstance->properties.clear();
	}

	void ControllerState::destroyProperty( InstanceProperty* pProperty )
	{
		if( pProperty->pGetCommand != nullptr )
		{
			m_controller.releaseCommand( pProperty->pGetCommand );
		}

		if( pProperty->pSetCommand != nullptr )
		{
			m_controller.releaseCommand( pProperty->pSetCommand );
		}
	}
}