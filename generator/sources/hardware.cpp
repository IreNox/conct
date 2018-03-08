#include "hardware.h"

#include "xml_helper.h"

#include "third_party/tinyxml2.h"

#include <iostream>

namespace conct
{
	Hardware::Hardware()
	{
	}

	bool Hardware::load( const std::string& fileName )
	{
		tinyxml2::XMLDocument document;
		if( document.LoadFile( fileName.c_str() ) != tinyxml2::XML_SUCCESS )
		{
			std::cout << "Error: Failed to load XML from '" << fileName << "'. Message: " << document.ErrorStr() << std::endl;
			return false;
		}

		tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "hardware" );
		if( pRootNode == nullptr )
		{
			std::cout << "Error: Failed to find 'hardware' root node in '" << fileName << "'." << std::endl;
			return false;
		}

		if( !loadStringValue( m_name, pRootNode, "name" ) )
		{
			std::cout << "Error: Failed to find hardware name in '" << fileName << "'." << std::endl;
			return false;
		}

		std::string runtime;
		if( !loadStringValue( runtime, pRootNode, "runtime" ) )
		{
			std::cout << "Error: Failed to find hardware runtime in '" << fileName << "'." << std::endl;
			return false;
		}

		if( runtime == "low" )
		{
			m_runtime = HardwareRuntime_Low;
		}
		else if( runtime == "high" )
		{
			m_runtime = HardwareRuntime_High;
		}
		else
		{
			std::cout << "Error: '" << runtime << "' is not a valid runtime." << std::endl;
			return false;
		}

		tinyxml2::XMLElement* pSpecsNode = pRootNode->FirstChildElement( "specifications" );
		if( pSpecsNode == nullptr )
		{
			std::cout << "Error: Failed to find 'specifications' node in '" << fileName << "'." << std::endl;
			return false;
		}

		return loadMemSizeValue( m_programDataSize, pSpecsNode, "program" ) &&
			loadMemSizeValue( m_ramSize, pSpecsNode, "ram" ) &&
			loadMemSizeValue( m_flashSize, pSpecsNode, "flash" );
	}
}
