#include "device.h"

#include "hardware_collection.h"
#include "service_collection.h"
#include "type_collection.h"

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
		m_pHardware = hardwareCollection.findHardwareByName( pHardwareNode->Value() );
		if( m_pHardware == nullptr )
		{
			std::cout << "Error: Failed to find hardware with name '" << pHardwareNode->Value() << "'." << std::endl;
			return false;
		}

		return true;
	}
}
