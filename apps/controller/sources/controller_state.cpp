#include "controller_state.h"

#include "conct_interface_type.h"

namespace conct
{
	bool ControllerState::setup( ControllerConfig& config, RuntimeHigh& runtime )
	{
		const Path exePath = Path::getExecutablePath();
		const Path basePath = exePath.getParent().getParent().getParent().getParent().getParent().getParent();
		const Path typesPath = basePath.push( "config/types"_s );

		if( !m_types.load( typesPath ) )
		{
			return false;
		}

		m_pRuntime	= &runtime;
		m_pConfig	= &config;

		m_controller.setup( runtime );

		return true;
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
		//DeviceVector lostDevices = m_devices;
		//m_devices.clear();

		Vector< DeviceConnection > devices;
		m_pRuntime->getDevices( devices );

		DeviceAddress address;
		for( const DeviceConnection& connection : devices )
		{
			runtime::clearDeviceAddress( address );
			runtime::pushToDeviceAddress( address, connection.id );

			ConnectedDevice* pDevice = nullptr;
			for( size_t i = 0u; i < m_devices.getLength(); ++i )
			{
				ConnectedDevice* pCurrentDevice = m_devices[ i ];
				if( !runtime::isDeviceAddressEquals( address, pCurrentDevice->address ) )
				{
					continue;
				}

				pDevice = pCurrentDevice;
				//lostDevices.eraseUnsortedByIndex( i );
				break;
			}

			if( pDevice == nullptr )
			{
				pDevice = createDevice( address );
				pDevice->allowRoutering = true;
				m_devices.pushBack( pDevice );
			}
		}

		//for( ConnectedDevice* pLostDevice : lostDevices )
		//{
		//	destroyDevice( pLostDevice );
		//}

		for( size_t i = 0u; i < m_devices.getLength(); ++i )
		{
			updateDevice( m_devices[ i ] );
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
			destroyInstance( instance );
		}

		delete pDevice;
	}

	void ControllerState::updateDevice( ConnectedDevice* pDevice )
	{
		if( pDevice->pNameCommand != nullptr &&
			pDevice->pNameCommand->isFinish() )
		{
			if( pDevice->pNameCommand->isOk() )
			{
				pDevice->name.assign( pDevice->pNameCommand->getValue().getString() );
			}

			m_controller.releaseCommand( pDevice->pNameCommand );
			pDevice->pNameCommand = nullptr;
		}

		if( pDevice->pInstancesCommand != nullptr &&
			pDevice->pInstancesCommand->isFinish() )
		{
			if( pDevice->pInstancesCommand->isOk() )
			{
				const ArrayView< Instance > instances = pDevice->pInstancesCommand->getValue().getArray< Instance >();
				for( const Instance& instanceData : instances )
				{
					if( instanceData.type == 0x6435u )
					{
						if( !pDevice->allowRoutering )
						{
							continue;
						}

						RemoteInstance remoteInstance;
						remoteInstance.address	= pDevice->address;
						remoteInstance.id		= instanceData.id;

						pDevice->pDevicesCommand = m_controller.getProperty( remoteInstance, "ConnectedDevices" );
						continue;
					}

					DeviceInstance& instance = pDevice->instances.pushBack();
					createInstance( instance, instanceData, *pDevice );
				}
			}

			m_controller.releaseCommand( pDevice->pInstancesCommand );
			pDevice->pInstancesCommand = nullptr;
		}

		if( pDevice->pDevicesCommand != nullptr &&
			pDevice->pDevicesCommand->isFinish() )
		{
			if( pDevice->pDevicesCommand->isOk() )
			{
				const ArrayView< DeviceConnection > connectedDevices = pDevice->pDevicesCommand->getValue().getArray< DeviceConnection >();
				for( const DeviceConnection& connectedDevice : connectedDevices )
				{
					DeviceAddress address = pDevice->address;
					runtime::pushToDeviceAddress( address, connectedDevice.id );

					if( m_pRuntime->isThisDevice( address ) )
					{
						continue;
					}

					ConnectedDevice* pNewDevice = createDevice( address );
					m_devices.pushBack( pNewDevice );
				}
			}

			m_controller.releaseCommand( pDevice->pDevicesCommand );
			pDevice->pDevicesCommand = nullptr;
		}


		for( DeviceInstance& instance : pDevice->instances )
		{
			updateInstance( instance, *pDevice );
		}
	}

	void ControllerState::createInstance( DeviceInstance& instance, const Instance& data, const ConnectedDevice& device )
	{
		instance.instance	= data;
		instance.pType		= m_types.findInterfaceByCrc( data.type );

		if( instance.pType == nullptr )
		{
			return;
		}

		for( const InterfaceProperty& propData : instance.pType->getProperties() )
		{
			InstanceProperty& prop = instance.properties.pushBack();
			createProperty( prop, propData, device, instance );
		}
	}

	void ControllerState::destroyInstance( DeviceInstance& instance )
	{
		for( InstanceProperty& prop : instance.properties )
		{
			destroyProperty( prop );
		}

		instance.properties.clear();
	}

	void ControllerState::updateInstance( DeviceInstance& instance, const ConnectedDevice& device )
	{
		for( InstanceProperty& prop : instance.properties )
		{
			updateProperty( prop, device, instance );
		}
	}

	void ControllerState::createProperty( InstanceProperty& prop, const InterfaceProperty& data, const ConnectedDevice& device, const DeviceInstance& instance )
	{
		prop.pProperty	= &data;

		RemoteInstance remoteInstance;
		remoteInstance.address	= device.address;
		remoteInstance.id		= instance.instance.id;

		prop.pGetCommand = m_controller.getProperty( remoteInstance, data.name.toConstCharPointer() );
	}

	void ControllerState::destroyProperty( InstanceProperty& prop )
	{
		prop.pProperty = nullptr;

		if( prop.pGetCommand != nullptr )
		{
			m_controller.releaseCommand( prop.pGetCommand );
			prop.pGetCommand = nullptr;
		}

		if( prop.pSetCommand != nullptr )
		{
			m_controller.releaseCommand( prop.pSetCommand );
			prop.pSetCommand = nullptr;
		}

		prop.value.setVoid();
		prop.hasValueChanged = false;
	}

	void ControllerState::updateProperty( InstanceProperty& prop, const ConnectedDevice& device, const DeviceInstance& instance )
	{
		if( prop.pGetCommand != nullptr &&
			prop.pGetCommand->isFinish() )
		{
			if( prop.pGetCommand->isOk() )
			{
				prop.value = prop.pGetCommand->getValue();
			}

			m_controller.releaseCommand( prop.pGetCommand );
			prop.pGetCommand = nullptr;
		}

		if( prop.pSetCommand != nullptr &&
			prop.pSetCommand->isFinish() )
		{
			if( prop.pGetCommand->isFailure() )
			{
				// TODO: try again
				//prop.hasValueChanged = true;
			}

			m_controller.releaseCommand( prop.pSetCommand );
			prop.pSetCommand = nullptr;
		}

		if( prop.pGetCommand == nullptr &&
			prop.pSetCommand == nullptr &&
			prop.hasValueChanged )
		{
			RemoteInstance remoteInstance;
			remoteInstance.address	= device.address;
			remoteInstance.id		= instance.instance.id;

			prop.pSetCommand = m_controller.setProperty( remoteInstance, prop.pProperty->name.toConstCharPointer(), prop.value );
			prop.hasValueChanged = false;
		}
	}
}