#include "hardware.h"

#include "conct_pair.h"
#include "conct_path.h"
#include "conct_trace.h"
#include "conct_xml_helper.h"

#include "tinyxml2/tinyxml2.h"

namespace conct
{
	static const Pair< HardwareRuntime, const char* > s_hardwareRuntimeMapping[] =
	{
		{ HardwareRuntime_Low,	"low" },
		{ HardwareRuntime_High,	"high" }
	};
	static const ArrayView< Pair< HardwareRuntime, const char* > > s_hardwareRuntimeMappingView( s_hardwareRuntimeMapping, CONCT_COUNT( s_hardwareRuntimeMapping ) );
	CONCT_STATIC_ASSERT( CONCT_COUNT( s_hardwareRuntimeMapping ) == HardwareRuntime_Count );

	static const Pair< HardwareSystem, const char* > s_hardwareSystemMapping[] =
	{
		{ HardwareSystem_Android,	"android" },
		{ HardwareSystem_Arduino,	"arduino" },
		{ HardwareSystem_Esp,		"esp" },
		{ HardwareSystem_Linux,		"linux" },
		{ HardwareSystem_Windows,	"windows" }
	};
	static const ArrayView< Pair< HardwareSystem, const char* > > s_hardwareSystemMappingView( s_hardwareSystemMapping, CONCT_COUNT( s_hardwareSystemMapping ) );
	CONCT_STATIC_ASSERT( CONCT_COUNT( s_hardwareSystemMapping ) == HardwareSystem_Count );

	static const Pair< HardwareEnvironment, const char* > s_hardwareEnvironmentMapping[] =
	{
		{ HardwareEnvironment_Production,	"production" },
		{ HardwareEnvironment_Simulator,	"simulator" },
		{ HardwareEnvironment_Tests,		"tests" },
	};
	static const ArrayView< Pair< HardwareEnvironment, const char* > > s_hardwareEnvironmentMappingView( s_hardwareEnvironmentMapping, CONCT_COUNT( s_hardwareEnvironmentMapping ) );
	CONCT_STATIC_ASSERT( CONCT_COUNT( s_hardwareEnvironmentMapping ) == HardwareEnvironment_Count );

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

		if( !loadEnumValue( m_runtime, pRootNode, "runtime", s_hardwareRuntimeMappingView ) ||
			!loadEnumValue( m_system, pRootNode, "system", s_hardwareSystemMappingView ) ||
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

	const ArrayView< Pair< HardwareRuntime, const char* > >& getHardwareRuntimeMapping()
	{
		return s_hardwareRuntimeMappingView;
	}

	const ArrayView< Pair< HardwareSystem, const char* > >& getHardwareSystemMapping()
	{
		return s_hardwareSystemMappingView;
	}

	const ArrayView< Pair< HardwareEnvironment, const char* > >& getHardwareEnvironmentMapping()
	{
		return s_hardwareEnvironmentMappingView;
	}
}
