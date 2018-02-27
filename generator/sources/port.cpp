#include "port.h"

#include "xml_helper.h"

#include <third_party/tinyxml2.h>

#include <iostream>

namespace conct
{
	Port::Port()
	{
		m_mtu				= 0u;
		m_singleConnection	= false;
		m_multiConnection	= false;
		m_reliable			= false;
	}

	bool Port::load( const std::string& filename )
	{
		tinyxml2::XMLDocument document;
		if( document.LoadFile( filename.c_str() ) != tinyxml2::XML_SUCCESS )
		{
			std::cout << "Error: Failed to load XML from '" << filename << "'. Message: " << document.ErrorStr() << std::endl;
			return false;
		}

		tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "port" );
		if( pRootNode == nullptr )
		{
			std::cout << "Error: Failed to find 'port' root node in '" << filename << "'." << std::endl;
			return false;
		}

		if( !loadStringValue( m_name, pRootNode, "name" ) )
		{
			return false;
		}

		tinyxml2::XMLElement* pConnectionNode = pRootNode->FirstChildElement( "connection" );
		if( pConnectionNode == nullptr )
		{
			std::cout << "Error: Failed to find 'connection' root node in '" << filename << "'." << std::endl;
			return false;
		}

		return loadMemSizeValue( m_mtu, pConnectionNode, "mtu" ) &&
			loadBooleanValue( m_singleConnection, pConnectionNode, "single_device" ) &&
			loadBooleanValue( m_multiConnection, pConnectionNode, "multi_device" ) &&
			loadBooleanValue( m_reliable, pConnectionNode, "reliable" );
	}
}
