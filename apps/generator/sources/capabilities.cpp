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
	TIKI_STATIC_ASSERT( TIKI_ARRAY_COUNT( s_aCapabilityNames ) == (uintsize)Capability::Count );

	bool Capabilities::metRequirements( const Capabilities& requirements, DynamicArray< Capability >* pMissingCapabilities /*= nullptr*/ ) const
	{
		bool result = true;
		for( uintsize i = 0u; i < (uintsize)Capability::Count; ++i )
		{
			const Capability c = (Capability)i;

			if( requirements.hasCapability( c ) &&
				!hasCapability( c ) )
			{
				if( pMissingCapabilities )
				{
					pMissingCapabilities->pushBack( c );
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
		for( uint i = 0u; i < (uintsize)Capability::Count; ++i )
		{
			bool has;
			if( !loadBooleanValue( has, pNode, s_aCapabilityNames[ i ] ) )
			{
				return false;
			}

			if( has )
			{
				m_capabilities.set( (Capability)i );
			}
		}

		return true;
	}

	const char* getCapabilityName( Capability value )
	{
		return s_aCapabilityNames[ (uintsize)value ];
	}
}
