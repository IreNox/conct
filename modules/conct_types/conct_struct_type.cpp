#include "conct_struct_type.h"

#include "conct_trace.h"
#include "conct_type_collection.h"
#include "conct_xml_helper.h"

#include <tinyxml2/tinyxml2.h>

namespace conct
{
	StructType::StructType()
	{
	}

	void StructType::create( const Path& fileName, const DynamicString& namespaceVar, const DynamicString& name )
	{
		Type::create( fileName, namespaceVar, name, name, TypeKind_Struct, ValueType_Struct, false );
	}

	bool StructType::load( TypeCollection& typeCollection )
	{
		tinyxml2::XMLDocument document;
		if( !loadDocument( document, m_fileName ) )
		{
			return false;
		}

		tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "struct" );
		if( pRootNode == nullptr )
		{
			trace::write( "Error: Failed to find root node in '"_s + m_fileName.getGenericPath() + "'." + "\n" );
			return false;
		}

		if( !loadInternal( pRootNode ) )
		{
			return false;
		}

		tinyxml2::XMLElement* pFieldsNode = pRootNode->FirstChildElement( "fields" );
		if( pFieldsNode != nullptr )
		{
			tinyxml2::XMLElement* pFieldNode = pFieldsNode->FirstChildElement( "field" );
			while( pFieldNode != nullptr )
			{
				DynamicString propertyName;
				const Type* pType = nullptr;
				if( !loadStringValue( propertyName, pFieldNode, "name" ) ||
					!loadTypeValue( &pType, pFieldNode, "type", getNamespace(), typeCollection ) )
				{
					return false;
				}

				if( pType->getKind() == TypeKind_Array ||
					pType->getKind() == TypeKind_Interface )
				{
					trace::fileError( m_fileName.getGenericPath(), pFieldNode->GetLineNum(), "Error: It is not allowed to use arrays or interfaces in structs.\n"_s );
					return false;
				}

				StructField field;
				field.name		= propertyName;
				field.pType		= pType;
				m_fields.pushBack( field );
				pushDependingType( pType );

				pFieldNode = pFieldNode->NextSiblingElement( "field" );
			}
		}

		return true;
	}
}
