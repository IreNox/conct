#include "hardware.h"

#include "conct_trace.h"
#include "conct_xml_helper.h"

#include "tinyxml2/tinyxml2.h"

namespace conct
{
	static const Pair< HardwareSystem, const char* > s_hardwareSystemMapping[] =
	{
		{ HardwareSystem::Computer,	"computer" },
		{ HardwareSystem::Android,	"android" },
		{ HardwareSystem::Esp,		"esp" }
	};
	static const ArrayView< const Pair< HardwareSystem, const char* > > s_hardwareSystemMappingView( s_hardwareSystemMapping, TIKI_ARRAY_COUNT( s_hardwareSystemMapping ) );
	TIKI_STATIC_ASSERT( TIKI_ARRAY_COUNT( s_hardwareSystemMapping ) == (uintsize)HardwareSystem::Count );

	static const Pair< HardwareEnvironment, const char* > s_hardwareEnvironmentMapping[] =
	{
		{ HardwareEnvironment::Production,	"production" },
		{ HardwareEnvironment::Simulator,	"simulator" },
		{ HardwareEnvironment::Tests,		"tests" },
	};
	static const ArrayView< const Pair< HardwareEnvironment, const char* > > s_hardwareEnvironmentMappingView( s_hardwareEnvironmentMapping, TIKI_ARRAY_COUNT( s_hardwareEnvironmentMapping ) );
	TIKI_STATIC_ASSERT( TIKI_ARRAY_COUNT( s_hardwareEnvironmentMapping ) == (uintsize)HardwareEnvironment::Count );

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

		const tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "hardware" );
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

		if( !loadEnumValue( m_system, pRootNode, "system", s_hardwareSystemMappingView ) ||
			!loadEnumValue( m_environment, pRootNode, "environment", s_hardwareEnvironmentMappingView ) )
		{
			return false;
		}

		const tinyxml2::XMLElement* pMemoryNode = pRootNode->FirstChildElement( "memory" );
		if( pMemoryNode == nullptr )
		{
			trace::write( "Error: Failed to find 'memory' node in '"_s + fileName.getGenericPath() + "'." + "\n" );
			return false;
		}

		bool dynamicMemory = false;
		if( !loadBooleanValue( dynamicMemory, pMemoryNode, "dynamic" ) )
		{
			return false;
		}

		if( dynamicMemory )
		{
			m_programDataSize	= (uintptr)-1;
			m_ramSize			= (uintptr)-1;
			m_flashSize			= (uintptr)-1;
		}
		else
		{
			if( !loadMemSizeValue( m_programDataSize, pMemoryNode, "program" ) ||
				!loadMemSizeValue( m_ramSize, pMemoryNode, "ram" ) ||
				!loadMemSizeValue( m_flashSize, pMemoryNode, "flash" ) )
			{
				return false;
			}
		}

		const tinyxml2::XMLElement* pCapabilitiesNode = pRootNode->FirstChildElement( "capabilities" );
		if( pCapabilitiesNode == nullptr )
		{
			trace::write( "Error: Failed to find 'capabilities' node in '"_s + fileName.getGenericPath() + "'." + "\n" );
			return false;
		}

		return m_capabilities.load( pCapabilitiesNode );
	}

	const ArrayView< const Pair< HardwareSystem, const char* > >& getHardwareSystemMapping()
	{
		return s_hardwareSystemMappingView;
	}

	const ArrayView< const Pair< HardwareEnvironment, const char* > >& getHardwareEnvironmentMapping()
	{
		return s_hardwareEnvironmentMappingView;
	}
}
