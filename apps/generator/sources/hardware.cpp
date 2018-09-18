#include "hardware.h"

#include "conct_path.h"
#include "conct_trace.h"
#include "conct_xml_helper.h"

#include "tinyxml2/tinyxml2.h"

namespace conct
{
	Hardware::Hardware()
	{
	}

	bool Hardware::load( const Path& fileName )
	{
		tinyxml2::XMLDocument document;
		if( !loadDocument( document, fileName ) )
		{
			return false;
		}

		tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "hardware" );
		if( pRootNode == nullptr )
		{
			trace::write( "Error: Failed to find 'hardware' root node in '"_s + fileName.getGenericPath() + "'." + "\n" );
			return false;
		}

		if( !loadStringValue( m_name, pRootNode, "name" ) )
		{
			trace::write( "Error: Failed to find hardware name in '"_s + fileName.getGenericPath() + "'." + "\n" );
			return false;
		}

		DynamicString runtime;
		if( !loadStringValue( runtime, pRootNode, "runtime" ) )
		{
			trace::write( "Error: Failed to find hardware runtime in '"_s + fileName.getGenericPath() + "'." + "\n" );
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
			trace::write( "Error: '"_s + runtime + "' is not a valid runtime." + "\n" );
			return false;
		}

		tinyxml2::XMLElement* pSpecsNode = pRootNode->FirstChildElement( "specifications" );
		if( pSpecsNode == nullptr )
		{
			trace::write( "Error: Failed to find 'specifications' node in '"_s + fileName.getGenericPath() + "'." + "\n" );
			return false;
		}

		return loadMemSizeValue( m_programDataSize, pSpecsNode, "program" ) &&
			loadMemSizeValue( m_ramSize, pSpecsNode, "ram" ) &&
			loadMemSizeValue( m_flashSize, pSpecsNode, "flash" );
	}
}
