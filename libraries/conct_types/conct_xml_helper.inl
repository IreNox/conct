#pragma  once

namespace conct
{
	template< class TEnum >
	bool loadEnumValue( TEnum& target, const tinyxml2::XMLElement* pNode, const char* pName, const ArrayView< Pair< TEnum, const char*> >& mapping, bool ignoreMissing /*= false */ )
	{
		const char* pStringValue = pNode->Attribute( pName );
		if( pStringValue == nullptr )
		{
			const tinyxml2::XMLElement* pChildNode = pNode->FirstChildElement( pName );
			if( pChildNode != nullptr )
			{
				pStringValue = pChildNode->GetText();
			}

			if( pStringValue == nullptr )
			{
				if( !ignoreMissing )
				{
					traceNodeError( pNode, "Error: Failed to load value '"_s + pName + "'.\n" );
				}
				return false;
			}
		}

		for( const Pair< TEnum, const char* >& kvp : mapping )
		{
			if( isStringEquals( pStringValue, kvp.value ) )
			{
				target = kvp.key;
				return true;
			}
		}

		traceNodeError( pNode, "Error: value '"_s + pStringValue + "' is not a valid for '" + pName + "'.\n" );
		return false;
	}
}
