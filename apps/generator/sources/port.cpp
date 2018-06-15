#include "port.h"

#include "conct_path.h"
#include "conct_xml_helper.h"
#include "conct_trace.h"

#include <tinyxml2/tinyxml2.h>

namespace conct
{
	Port::Port()
	{
		m_mtu				= 0u;
		m_singleConnection	= false;
		m_multiConnection	= false;
		m_reliable			= false;
	}

	bool Port::load( const Path& filename )
	{
		tinyxml2::XMLDocument document;
		if( document.LoadFile( filename.getNativePath().toConstCharPointer() ) != tinyxml2::XML_SUCCESS )
		{
			trace::write( "Error: Failed to load XML from '"_s + filename.getGenericPath() + "'. Message: " + document.ErrorStr() + "\n" );
			return false;
		}

		tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "port" );
		if( pRootNode == nullptr )
		{
			trace::write( "Error: Failed to find 'port' root node in '"_s + filename.getGenericPath() + "'." + "\n" );
			return false;
		}

		if( !loadStringValue( m_name, pRootNode, "name" ) ||
			!loadStringValue( m_library, pRootNode, "library" )  ||
			!loadStringValue( m_class, pRootNode, "class" ) )
		{
			return false;
		}

		tinyxml2::XMLElement* pConnectionNode = pRootNode->FirstChildElement( "connection" );
		if( pConnectionNode == nullptr )
		{
			trace::write( "Error: Failed to find 'connection' root node in '"_s + filename.getGenericPath() + "'." + "\n" );
			return false;
		}

		return loadMemSizeValue( m_mtu, pConnectionNode, "mtu" ) &&
			loadBooleanValue( m_singleConnection, pConnectionNode, "single_device" ) &&
			loadBooleanValue( m_multiConnection, pConnectionNode, "multi_device" ) &&
			loadBooleanValue( m_reliable, pConnectionNode, "reliable" );
	}
}
