#include "xml_helper.h"

#include "array.h"
#include "interface.h"
#include "type_collection.h"

#include <third_party/tinyxml2.h>

#include <iostream>

namespace conct
{
	bool loadBooleanValue( bool& target, tinyxml2::XMLElement* pNode, const char* pName )
	{
		const char* pAttributeValue = pNode->Attribute( pName );
		if( pAttributeValue != nullptr )
		{
			const std::string attributeValue = pAttributeValue;
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

			std::cout << "Error: '" << attributeValue << "' is not an valid boolean attribute value for '" << pName << "' from '" << pNode->Name() << "'." << std::endl;
			return false;
		}

		target = ( pNode->FirstChildElement( pName ) != nullptr );
		return true;
	}

	bool loadStringValue( std::string& target, tinyxml2::XMLElement* pNode, const char* pName )
	{
		const char* pAttributeValue = pNode->Attribute( pName );
		if( pAttributeValue != nullptr )
		{
			target = pAttributeValue;
			return true;
		}

		tinyxml2::XMLElement* pChildNode = pNode->FirstChildElement( pName );
		if( pChildNode != nullptr )
		{
			const char* pText = pChildNode->GetText();
			if( pText != nullptr )
			{
				target = pText;
				return true;
			}
		}

		std::cout << "Error: Failed to load value '" << pName << "' from '" << pNode->Name() << "'." << std::endl;
		return false;
	}

	bool loadMemSizeValue( size_t& target, tinyxml2::XMLElement* pNode, const char* pName )
	{
		int64_t value = 0u;
		if( pNode->QueryInt64Attribute( pName, &value ) == tinyxml2::XML_SUCCESS )
		{
			target = (size_t)value;
			return true;
		}

		tinyxml2::XMLElement* pChildNode = pNode->FirstChildElement( pName );
		if( pChildNode != nullptr && pChildNode->QueryInt64Text( &value ) == tinyxml2::XML_SUCCESS )
		{
			target = (size_t)value;
			return true;
		}

		std::cout << "Error: Failed to load size_t value '" << pName << "' from '" << pNode->Name() << "'." << std::endl;
		return false;
	}

	bool loadTypeValue( const Type** ppType, tinyxml2::XMLElement* pNode, const char* pName, const std::string& referenceNamespace, TypeCollection& typeCollection, bool ignoreMissing /*= false*/ )
	{
		const char* pAttributeValue = pNode->Attribute( pName );
		if( pAttributeValue != nullptr )
		{
			*ppType = typeCollection.findType( pAttributeValue, referenceNamespace );
			if( *ppType == nullptr )
			{
				std::cout << "Error: Failed to find type with name '" << pAttributeValue << "' in '" << referenceNamespace << "'." << std::endl;
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
				*ppType = typeCollection.findType( pText, referenceNamespace );
				if( *ppType == nullptr )
				{
					std::cout << "Error: Failed to find type with name '" << pText << "' in '" << referenceNamespace << "'." << std::endl;
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
			std::cout << "Error: Failed to load type value '" << pName << "' from '" << pNode->Name() << "'." << std::endl;
		}
		return false;
	}

	bool loadInterfaceValue( const Interface** ppInterface, tinyxml2::XMLElement* pNode, const char* pName, const std::string& referenceNamespace, TypeCollection& typeCollection, bool ignoreMissing /*= false */ )
	{
		const Type* pType = nullptr;
		if( !loadTypeValue( &pType, pNode, pName, referenceNamespace, typeCollection, ignoreMissing ) )
		{
			return false;
		}

		if( pType->getDescription() != TypeDescription_Interface )
		{
			std::cout << "Error: '" << pType->getFullName() << "' is not an interface. loaded type value '" << pName << "' from '" << pNode->Name() << "'." << std::endl;
			return false;
		}

		*ppInterface = static_cast< const Interface* >( pType );
		return true;
	}
}
