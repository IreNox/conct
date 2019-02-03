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

	bool Capabilities::metRequirements( const Capabilities& requirements, UnsortedSet< Capability >* pMissingCapabilities /*= nullptr*/ ) const
	{
		bool result = true;
		for( Capability c : requirements.getCapabilities() )
		{
			if( !hasCapability( c ) )
			{
				if( pMissingCapabilities != nullptr )
				{
					pMissingCapabilities->insert( c );
				}
				result = false;
			}
		}

		return result;
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

	const char* getCapabilityName( Capability value )
	{
		return s_aCapabilityNames[ value ];
	}
}
