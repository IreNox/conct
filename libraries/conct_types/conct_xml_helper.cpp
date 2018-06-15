#include "conct_xml_helper.h"

#include "conct_array_type.h"
#include "conct_dynamic_string.h"
#include "conct_interface_type.h"
#include "conct_trace.h"
#include "conct_type_collection.h"

#include <tinyxml2/tinyxml2.h>

namespace conct
{
	bool loadBooleanValue( bool& target, tinyxml2::XMLElement* pNode, const char* pName )
	{
		const char* pAttributeValue = pNode->Attribute( pName );
		if( pAttributeValue != nullptr )
		{
			const DynamicString attributeValue = DynamicString( pAttributeValue );
			if( attributeValue == pName || attributeValue == "1" || attributeValue == "true" )
			{
				target = true;
				return true;
			}
			else if( attributeValue == "0" || attributeValue == "false" )
			{
				target = false;
				return true;
			}

			trace::write( "Error: '"_s + attributeValue + "' is not an valid boolean attribute value for '" + pName + "' from '" + pNode->Name() + "'." + "\n" );
			return false;
		}

		target = ( pNode->FirstChildElement( pName ) != nullptr );
		return true;
	}

	bool loadStringValue( DynamicString& target, tinyxml2::XMLElement* pNode, const char* pName )
	{
		const char* pAttributeValue = pNode->Attribute( pName );
		if( pAttributeValue != nullptr )
		{
			target = DynamicString( pAttributeValue );
			return true;
		}

		tinyxml2::XMLElement* pChildNode = pNode->FirstChildElement( pName );
		if( pChildNode != nullptr )
		{
			const char* pText = pChildNode->GetText();
			if( pText != nullptr )
			{
				target = DynamicString( pText );
				return true;
			}
		}

		trace::write( "Error: Failed to load value '"_s + pName + "' from '" + pNode->Name() + "'." + "\n" );
		return false;
	}

	bool loadMemSizeValue( uintreg& target, tinyxml2::XMLElement* pNode, const char* pName )
	{
		int64_t value = 0u;
		if( pNode->QueryInt64Attribute( pName, &value ) == tinyxml2::XML_SUCCESS )
		{
			target = ( uintreg )value;
			return true;
		}

		tinyxml2::XMLElement* pChildNode = pNode->FirstChildElement( pName );
		if( pChildNode != nullptr && pChildNode->QueryInt64Text( &value ) == tinyxml2::XML_SUCCESS )
		{
			target = ( uintreg )value;
			return true;
		}

		trace::write( "Error: Failed to load uintreg value '"_s + pName + "' from '" + pNode->Name() + "'." + "\n" );
		return false;
	}

	bool loadTypeValue( const Type** ppType, tinyxml2::XMLElement* pNode, const char* pName, const DynamicString& referenceNamespace, TypeCollection& typeCollection, bool ignoreMissing /*= false*/ )
	{
		const char* pAttributeValue = pNode->Attribute( pName );
		if( pAttributeValue != nullptr )
		{
			*ppType = typeCollection.findType( DynamicString( pAttributeValue ), referenceNamespace );
			if( *ppType == nullptr )
			{
				trace::write( "Error: Failed to find type with name '"_s + pAttributeValue + "' in '" + referenceNamespace + "'." + "\n" );
				return false;
			}

			return true;
		}

		tinyxml2::XMLElement* pChildNode = pNode->FirstChildElement( pName );
		if( pChildNode != nullptr )
		{
			const char* pText = pChildNode->GetText();
			if( pText != nullptr )
			{
				*ppType = typeCollection.findType( DynamicString( pText ), referenceNamespace );
				if( *ppType == nullptr )
				{
					trace::write( "Error: Failed to find type with name '"_s + pText + "' in '" + referenceNamespace + "'." + "\n" );
					return false;
				}

				return true;
			}
			else
			{
				tinyxml2::XMLElement* pArrayNode = pChildNode->FirstChildElement( "array" );
				if( pArrayNode != nullptr )
				{
					const Type* pInnerType = nullptr;
					if( loadTypeValue( &pInnerType, pArrayNode, "type", referenceNamespace, typeCollection, ignoreMissing ) )
					{
						*ppType = typeCollection.makeArray( pInnerType );
						return true;
					}
				}
			}
		}

		if( !ignoreMissing )
		{
			trace::write( "Error: Failed to load type value '"_s + pName + "' from '" + pNode->Name() + "'." + "\n" );
		}
		return false;
	}

	bool loadInterfaceValue( const InterfaceType** ppInterface, tinyxml2::XMLElement* pNode, const char* pName, const DynamicString& referenceNamespace, TypeCollection& typeCollection, bool ignoreMissing /*= false */ )
	{
		const Type* pType = nullptr;
		if( !loadTypeValue( &pType, pNode, pName, referenceNamespace, typeCollection, ignoreMissing ) )
		{
			return false;
		}

		if( pType->getDescription() != TypeDescription_Interface )
		{
			trace::write( "Error: '"_s + pType->getFullName() + "' is not an interface. loaded type value '" + pName + "' from '" + pNode->Name() + "'." + "\n" );
			return false;
		}

		*ppInterface = static_cast< const InterfaceType* >( pType );
		return true;
	}
}
