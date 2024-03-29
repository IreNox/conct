#include "conct_xml_helper.h"

#include "conct_array_type.h"
#include "conct_dynamic_string.h"
#include "conct_interface_type.h"
#include "conct_trace.h"
#include "conct_type_collection.h"

#include <tinyxml2/tinyxml2.h>

namespace conct
{
	bool loadDocument( tinyxml2::XMLDocument& document, const Path& path )
	{
		if( document.LoadFile( path.getNativePath().toConstCharPointer() ) != tinyxml2::XML_SUCCESS )
		{
			trace::fileError( path.getNativePath(), document.ErrorLineNum(), "Error: Failed to load XML. Message: "_s + document.ErrorStr() + "\n" );
			return false;
		}

		tinyxml2::XMLUnknown* pUserData = document.NewUnknown( path.getNativePath().toConstCharPointer() );
		document.SetUserData( pUserData );

		return true;
	}

	void traceNodeError( const tinyxml2::XMLElement* pNode, const char* pText )
	{
		traceNodeError( pNode, DynamicString( pText ) );
	}

	void traceNodeError( const tinyxml2::XMLElement* pNode, const DynamicString& text )
	{
		tinyxml2::XMLUnknown* pUserData = static_cast<tinyxml2::XMLUnknown*>(pNode->GetDocument()->GetUserData());
		trace::fileError( pUserData->Value(), pNode->GetLineNum(), text );
	}

	bool loadBooleanValue( bool& target, const tinyxml2::XMLElement* pNode, const char* pName )
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

			traceNodeError( pNode, "Error: '"_s + attributeValue + "' is not an valid boolean attribute value for '" + pName + "'.\n" );
			return false;
		}

		target = ( pNode->FirstChildElement( pName ) != nullptr );
		return true;
	}

	bool loadStringValue( DynamicString& target, const tinyxml2::XMLElement* pNode, const char* pName, bool ignoreMissing /*= false*/ )
	{
		const char* pAttributeValue = pNode->Attribute( pName );
		if( pAttributeValue != nullptr )
		{
			target = DynamicString( pAttributeValue );
			return true;
		}

		const tinyxml2::XMLElement* pChildNode = pNode->FirstChildElement( pName );
		if( pChildNode != nullptr )
		{
			const char* pText = pChildNode->GetText();
			if( pText != nullptr )
			{
				target = DynamicString( pText );
				return true;
			}
		}

		if( !ignoreMissing )
		{
			traceNodeError( pNode, "Error: Failed to load value '"_s + pName + "'.\n" );
		}
		return false;
	}

	bool loadMemSizeValue( uintreg& target, const tinyxml2::XMLElement* pNode, const char* pName, bool ignoreMissing /*= false*/ )
	{
		sint64 value = 0u;
		if( !loadIntegerValue( value, pNode, pName, ignoreMissing ) )
		{
			return false;
		}

		target = (uintreg)value;
		return true;
	}

	bool loadIntegerValue( sint64& target, const tinyxml2::XMLElement* pNode, const char* pName, bool ignoreMissing /*= false */ )
	{
		int64_t value = 0u;
		if( pNode->QueryInt64Attribute( pName, &value ) == tinyxml2::XML_SUCCESS )
		{
			target = value;
			return true;
		}

		const tinyxml2::XMLElement* pChildNode = pNode->FirstChildElement( pName );
		if( pChildNode != nullptr && pChildNode->QueryInt64Text( &value ) == tinyxml2::XML_SUCCESS )
		{
			target = value;
			return true;
		}

		if( !ignoreMissing )
		{
			traceNodeError( pNode, "Error: Failed to load integer value '"_s + pName + "'.\n" );
		}
		return false;
	}

	bool loadTypeValue( const Type** ppType, const tinyxml2::XMLElement* pNode, const char* pName, const DynamicString& referenceNamespace, TypeCollection& typeCollection, bool ignoreMissing /*= false*/ )
	{
		const char* pAttributeValue = pNode->Attribute( pName );
		if( pAttributeValue != nullptr )
		{
			*ppType = typeCollection.findType( DynamicString( pAttributeValue ), referenceNamespace );
			if( *ppType == nullptr )
			{
				traceNodeError( pNode, "Error: Failed to find type with name '"_s + pAttributeValue + "' in '" + referenceNamespace + "'.\n" );
				return false;
			}

			return true;
		}

		const tinyxml2::XMLElement* pChildNode = pNode->FirstChildElement( pName );
		if( pChildNode != nullptr )
		{
			const char* pText = pChildNode->GetText();
			if( pText != nullptr )
			{
				*ppType = typeCollection.findType( DynamicString( pText ), referenceNamespace );
				if( *ppType == nullptr )
				{
					traceNodeError( pNode, "Error: Failed to find type with name '"_s + pText + "' in '" + referenceNamespace + "'.\n" );
					return false;
				}

				return true;
			}
			else
			{
				const tinyxml2::XMLElement* pArrayNode = pChildNode->FirstChildElement( "array" );
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
			traceNodeError( pNode, "Error: Failed to load type value '"_s + pName + "' from '" + pNode->Name() + "'.\n" );
		}
		return false;
	}

	bool loadInterfaceValue( const InterfaceType** ppInterface, const tinyxml2::XMLElement* pNode, const char* pName, const DynamicString& referenceNamespace, TypeCollection& typeCollection, bool ignoreMissing /*= false */ )
	{
		const Type* pType = nullptr;
		if( !loadTypeValue( &pType, pNode, pName, referenceNamespace, typeCollection, ignoreMissing ) )
		{
			return false;
		}

		if( pType->getKind() != TypeKind_Interface )
		{
			traceNodeError( pNode, "Error: '"_s + pType->getFullName() + "' is not an interface. loaded type value '" + pName + "'.\n" );
			return false;
		}

		*ppInterface = static_cast< const InterfaceType* >( pType );
		return true;
	}
}
