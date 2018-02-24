#include "device.h"

#include "hardware_collection.h"
#include "service_collection.h"
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

	bool Device::load( const std::string& fileName, HardwareCollection& hardwareCollection, ServiceCollection& serviceCollection, TypeCollection& typeCollection )
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

		tinyxml2::XMLElement* pHardwareNode = pRootNode->FirstChildElement( "hardware" );
		m_pHardware = hardwareCollection.findHardwareByName( pHardwareNode->GetText() );
		if( m_pHardware == nullptr )
		{
			std::cout << "Error: Failed to find hardware with name '" << pHardwareNode->GetText() << "'." << std::endl;
			return false;
		}

		tinyxml2::XMLElement* pServicesNode = pRootNode->FirstChildElement( "services" );
		if( pServicesNode == nullptr )
		{
			m_services = m_pHardware->getServices();
		}
		else
		{
			tinyxml2::XMLElement* pServiceNode = pServicesNode->FirstChildElement();
			while( pServiceNode != nullptr )
			{
				const Service* pService = serviceCollection.findServiceByTag( pServiceNode->Name() );
				if( pService == nullptr )
				{
					std::cout << "Error: Failed to find service with tag '" << pServiceNode->Name() << "'." << std::endl;
					return false;
				}

				m_services.push_back( pService );

				pServiceNode = pServiceNode->NextSiblingElement();
			}
		}

		tinyxml2::XMLElement* pInterfacesNode = pRootNode->FirstChildElement( "interfaces" );
		if( pInterfacesNode == nullptr )
		{
			std::cout << "Error: Failed to find 'interfaces' node in '" << fileName << "'." << std::endl;
			return false;
		}

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

			m_interfaces.push_back( pInterface );

			pInterfaceNode = pInterfaceNode->NextSiblingElement( "interface" );
		}

		if( m_interfaces.empty() )
		{
			std::cout << "Error: Device has no interfaces." << std::endl;
			return false;
		}

		return true;
	}
}
