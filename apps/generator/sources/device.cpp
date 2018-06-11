#include "device.h"

#include "hardware.h"
#include "hardware_collection.h"
#include "port_collection.h"

#include "conct_interface_type.h"
#include "conct_path.h"
#include "conct_type_collection.h"
#include "conct_xml_helper.h"

#include <tinyxml2/tinyxml2.h>

#include <iostream>

namespace conct
{
	Device::Device()
	{
		m_pHardware = nullptr;

	}

	bool Device::load( const Path& fileName, HardwareCollection& hardwareCollection, PortCollection& portCollection, TypeCollection& typeCollection )
	{
		tinyxml2::XMLDocument document;
		if( document.LoadFile( fileName.getNativePath().toConstCharPointer() ) != tinyxml2::XML_SUCCESS )
		{
			std::cout << "Error: Failed to load XML from '" << fileName.getGenericPath() << "'. Message: " << document.ErrorStr() << std::endl;
			return false;
		}

		tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "device" );
		if( pRootNode == nullptr )
		{
			std::cout << "Error: Failed to find 'device' root node in '" << fileName.getGenericPath() << "'." << std::endl;
			return false;
		}

		if( !loadStringValue( m_name, pRootNode, "name" ) )
		{
			return false;
		}

		DynamicString hardwareName;
		if( !loadStringValue( hardwareName, pRootNode, "hardware" ) )
		{
			std::cout << "Error: Failed to find hardware node." << std::endl;
			return false;
		}

		m_pHardware = hardwareCollection.findHardwareByName( hardwareName );
		if( m_pHardware == nullptr )
		{
			std::cout << "Error: Failed to find hardware with name '" << hardwareName << "'." << std::endl;
			return false;
		}

		tinyxml2::XMLElement* pPortsNode = pRootNode->FirstChildElement( "ports" );
		if( pPortsNode == nullptr )
		{
			std::cout << "Error: Failed to find 'ports' node." << std::endl;
			return false;
		}

		for( tinyxml2::XMLElement* pPortNode = pPortsNode->FirstChildElement( "port" ); pPortNode != nullptr; pPortNode = pPortNode->NextSiblingElement( "port" ) )
		{
			DevicePort port;

			DynamicString portType;
			if( !loadStringValue( portType, pPortNode, "type" ) )
			{
				std::cout << "Error: Failed to find port name." << std::endl;
				return false;
			}

			if( !loadMemSizeValue( port.index, pPortNode, "index" ) )
			{
				std::cout << "Error: Failed to find port index." << std::endl;
				return false;
			}

			port.pPort = portCollection.findPortByName( portType );
			if( port.pPort == nullptr )
			{
				std::cout << "Error: Failed to find port with tag '" << portType << "'." << std::endl;
				return false;
			}

			m_ports.pushBack( port );
		}

		if( m_ports.isEmpty() )
		{
			std::cout << "Error: Device has no ports." << std::endl;
			return false;
		}

		tinyxml2::XMLElement* pInterfacesNode = pRootNode->FirstChildElement( "interfaces" );
		if( pInterfacesNode != nullptr )
		{
			for( tinyxml2::XMLElement* pInterfaceNode = pInterfacesNode->FirstChildElement( "interface" ); pInterfaceNode != nullptr; pInterfaceNode = pInterfaceNode->NextSiblingElement( "interface" ) )
			{
				const InterfaceType* pInterface = nullptr;
				if( !loadInterfaceValue( &pInterface, pInterfaceNode, "type", ""_s, typeCollection ) )
				{
					return false;
				}

				m_interfaces.insert( pInterface );
			}
		}

		DeviceInstance deviceInstance;
		deviceInstance.id = m_instances.getCount();
		deviceInstance.pInterface = typeCollection.findInterface( "Core.Device"_s, ""_s );
		m_instances.pushBack( deviceInstance );
		m_interfaces.insert( deviceInstance.pInterface );
		m_proxies.insert( deviceInstance.pInterface );

		if( m_pHardware->getRuntime() == HardwareRuntime_High )
		{
			DeviceInstance routerInstance;
			routerInstance.id = m_instances.getCount();
			routerInstance.pInterface = typeCollection.findInterface( "Core.Router"_s, ""_s );
			m_instances.pushBack( routerInstance );
			m_interfaces.insert( routerInstance.pInterface );
			m_proxies.insert( routerInstance.pInterface );
		}

		tinyxml2::XMLElement* pInstancesNode = pRootNode->FirstChildElement( "instances" );
		if( pInstancesNode != nullptr )
		{
			for( tinyxml2::XMLElement* pInstanceNode = pInstancesNode->FirstChildElement( "instance" ); pInstanceNode != nullptr; pInstanceNode = pInstanceNode->NextSiblingElement( "instance" ) )
			{
				DeviceInstance instance;
				instance.id = m_instances.getCount();

				if( !loadInterfaceValue( &instance.pInterface, pInstanceNode, "type", ""_s, typeCollection ) )
				{
					return false;
				}

				if( instance.pInterface->getSingleton() )
				{
					for( const DeviceInstance& instance2 : m_instances )
					{
						if( instance.pInterface == instance2.pInterface )
						{
							std::cout << "Error: Device has more than one instance of a singleton interface. Type name: " << instance.pInterface->getFullName() << std::endl;
							return false;
						}
					}
				}

				m_instances.pushBack( instance );
				m_interfaces.insert( instance.pInterface );
				m_proxies.insert( instance.pInterface );
			}
		}

		return true;
	}
}
