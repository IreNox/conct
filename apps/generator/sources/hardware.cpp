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

		if( !loadEnumValue( m_runtime, pRootNode, "runtime", s_hardwareRuntimeMappingView ) ||
			!loadEnumValue( m_system, pRootNode, "system", s_hardwareSystemMappingView ) ||
			!loadEnumValue( m_environment, pRootNode, "environment", s_hardwareEnvironmentMappingView ) )
		{
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
