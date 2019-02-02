#include "capabilities.h"

#include "conct_xml_helper.h"

namespace conct
{
	static const char* s_aCapabilityNames[] =
	{
		"allocation",
		"i2c",
		"gpio",
		"spi",
		"sockets",
		"uart"
	};
	CONCT_STATIC_ASSERT( CONCT_COUNT( s_aCapabilityNames ) == Capability_Count );

	bool Capabilities::metRequirements( const Capabilities& requirements ) const
	{
		for( Capability c : requirements.getCapabilities() )
		{
			if( !hasCapability( c ) )
			{
				return false;
			}
		}

		return true;
	}

	Capabilities::Capabilities()
	{
	}

	bool Capabilities::load( const tinyxml2::XMLElement* pNode )
	{
		for( uint i = 0u; i < Capability_Count; ++i )
		{
			bool has;
			if( !loadBooleanValue( has, pNode, s_aCapabilityNames[ i ] ) )
			{
				return false;
			}

			if( has )
			{
				m_capabilities.insert( (Capability)i );
			}
		}

		return true;
	}
}
