#include "device.h"

#include "hardware_collection.h"
#include "port_collection.h"
#include "type_collection.h"
#include "xml_helper.h"

#include <third_party/tinyxml2.h>

#include <iostream>

namespace conct
{
	Device::Device()
	{
		m_pHardware = nullptr;
	}

	bool Device::load( const std::string& fileName, HardwareCollection& hardwareCollection, PortCollection& portCollection, TypeCollection& typeCollection )
	{
		tinyxml2::XMLDocument document;
		if( document.LoadFile( fileName.c_str() ) != tinyxml2::XML_SUCCESS )
		{
			std::cout << "Error: Failed to load XML from '" << fileName << "'. Message: " << document.ErrorStr() << std::endl;
			return false;
		}

		tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "device" );
		if( pRootNode == nullptr )
		{
			std::cout << "Error: Failed to find 'device' root node in '" << fileName << "'." << std::endl;
			return false;
		}

		if( !loadStringValue( m_name, pRootNode, "name" ) )
		{
			return false;
		}

		std::string hardwareName;
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

		tinyxml2::XMLElement* pPortNode = pPortsNode->FirstChildElement( "port" );
		while( pPortNode != nullptr )
		{
			DevicePort port;

			std::string portType;
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
				std::cout << "Error: Failed to find service with tag '" << portType << "'." << std::endl;
				return false;
			}

			m_ports.push_back( port );

			pPortNode = pPortNode->NextSiblingElement( "port" );
		}

		if( m_ports.empty() )
		{
			std::cout << "Error: Device has no ports." << std::endl;
			return false;
		}


		tinyxml2::XMLElement* pInterfacesNode = pRootNode->FirstChildElement( "interfaces" );
		if( pInterfacesNode != nullptr )
		{
			tinyxml2::XMLElement* pInterfaceNode = pInterfacesNode->FirstChildElement( "interface" );
			while( pInterfaceNode != nullptr )
			{
				std::string type;
				if( !loadStringValue( type, pInterfaceNode, "type" ) )
				{
					return false;
				}

				const Interface* pInterface = typeCollection.findInterface( type, "" );
				if( pInterface == nullptr )
				{
					std::cout << "Error: Failed to find interface with name '" << type << "'." << std::endl;
					return false;
				}

				m_interfaces.insert( pInterface );

				pInterfaceNode = pInterfaceNode->NextSiblingElement( "interface" );
			}
		}

		return true;
	}
}
