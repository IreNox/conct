#include "conct_struct_type.h"

#include "conct_type_collection.h"
#include "conct_xml_helper.h"

#include <tinyxml2/tinyxml2.h>

#include <iostream>

namespace conct
{
	StructType::StructType()
	{
	}

	void StructType::create( const Path& fileName, const DynamicString& namespaceVar, const DynamicString& name )
	{
		Type::create( namespaceVar, name, name, TypeDescription_Struct, ValueType_Struct );
		m_fileName = fileName;
	}

	bool StructType::load( TypeCollection& typeCollection )
	{
		tinyxml2::XMLDocument document;
		if( document.LoadFile( m_fileName.getNativePath().toConstCharPointer() ) != tinyxml2::XML_SUCCESS )
		{
			std::cout << "Error: Failed to load XML from '" << m_fileName.getGenericPath() << "'. Message: " << document.ErrorStr() << std::endl;
			return false;
		}

		tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "struct" );
		if( pRootNode == nullptr )
		{
			std::cout << "Error: Failed to find root node in '" << m_fileName.getGenericPath() << "'." << std::endl;
			return false;
		}

		tinyxml2::XMLElement* pPropertiesNode = pRootNode->FirstChildElement( "fields" );
		if( pPropertiesNode != nullptr )
		{
			tinyxml2::XMLElement* pPropertyNode = pPropertiesNode->FirstChildElement( "field" );
			while( pPropertyNode != nullptr )
			{
				DynamicString propertyName;
				const Type* pType = nullptr;
				if( !loadStringValue( propertyName, pPropertyNode, "name" ) ||
					!loadTypeValue( &pType, pPropertyNode, "type", getNamespace(), typeCollection ) )
				{
					return false;
				}

				StructField field;
				field.name		= propertyName;
				field.pType		= pType;
				m_fields.pushBack( field );
				pushDependingType( pType );

				pPropertyNode = pPropertyNode->NextSiblingElement( "field" );
			}
		}

		return true;
	}
}
