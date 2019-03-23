#include "conct_enum_type.h"

#include "conct_trace.h"
#include "conct_type_collection.h"
#include "conct_xml_helper.h"

#include <tinyxml2/tinyxml2.h>

namespace conct
{
	EnumType::EnumType()
	{
	}

	void EnumType::create( const Path& fileName, const DynamicString& namespaceVar, const DynamicString& name )
	{
		Type::create( namespaceVar, name, name, TypeDescription_Enum, ValueType_Integer );
		m_fileName = fileName;
	}

	bool EnumType::load( TypeCollection& typeCollection )
	{
		tinyxml2::XMLDocument document;
		if( !loadDocument( document, m_fileName ) )
		{
			return false;
		}

		tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "enum" );
		if( pRootNode == nullptr )
		{
			trace::write( "Error: Failed to find root node in '"_s + m_fileName.getGenericPath() + "'." + "\n" );
			return false;
		}

		tinyxml2::XMLElement* pValuesNode = pRootNode->FirstChildElement( "values" );
		if( pValuesNode != nullptr )
		{
			sint64 lastValue = 0u;
			for( tinyxml2::XMLElement* pValueNode = pValuesNode->FirstChildElement( "value" ); pValueNode != nullptr; pValueNode = pValueNode->NextSiblingElement( "value" ) )
			{
				DynamicString valueName;
				if( !loadStringValue( valueName, pValueNode, "name" ) )
				{
					return false;
				}

				sint64 value = lastValue + 1u;
				loadIntegerValue( value, pValueNode, "type", true );

				EnumValue enumValue;
				enumValue.name		= valueName;
				enumValue.value		= value;
				m_values.pushBack( enumValue );

				lastValue = value;
			}
		}

		return true;
	}
}
