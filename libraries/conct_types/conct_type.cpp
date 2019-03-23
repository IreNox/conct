#include "conct_type.h"

#include "conct_array_type.h"
#include "conct_ascii.h"
#include "conct_crc16.h"
#include "conct_trace.h"
#include "conct_xml_helper.h"

namespace conct
{
	Type::Type()
	{
		m_kind		= TypeKind_Value;
		m_valueType	= ValueType_Void;
		m_typeCrc	= 0u;
		m_internal	= false;
	}

	void Type::create( const Path& fileName, const DynamicString& namespaceVar, const DynamicString& name, const DynamicString& cppName, TypeKind kind, ValueType valueType, bool internal )
	{
		m_fileName	= fileName;
		m_namespace	= namespaceVar;
		m_name		= name;
		m_cppName	= cppName;
		m_kind		= kind;
		m_valueType	= valueType;
		m_internal	= internal;

		if( !m_namespace.isEmpty() )
		{
			m_fullName = m_namespace + ".";
		}
		m_fullName += m_name;

		switch( kind )
		{
		case TypeKind_Array:
			m_headerFilename = "conct_array_view.h"_s;
			break;

		case TypeKind_Value:
			m_headerFilename = "conct_core.h"_s;
			break;

		default:
			m_headerFilename = getFilename( "", "h" );
			break;
		}

		m_typeCrc = calculateCrc16( m_fullName.toConstCharPointer(), m_fullName.getLength() );
	}

	bool Type::loadInternal( const tinyxml2::XMLElement* pRootNode )
	{
		const tinyxml2::XMLElement* pInternalNode = pRootNode->FirstChildElement( "internal" );
		if( pInternalNode != nullptr )
		{
			if( !loadStringValue( m_headerFilename, pInternalNode, "include" ) )
			{
				trace::write( "Error: Internal type data not complete type of '"_s + getFullName() + "' in '" + m_fileName.getGenericPath() + "'." + "\n" );
				return false;
			}

			m_internal = true;
		}

		return true;
	}

	TypeCrc Type::getCrc() const
	{
		return m_typeCrc;
	}

	void Type::pushDependingType( const Type* pType )
	{
		m_dependingTypes.insert( pType );

		if( pType->getKind() == TypeKind_Array )
		{
			const ArrayType* pArrayType = static_cast< const ArrayType* >( pType );
			m_dependingTypes.insert( pArrayType->getBaseType() );
		}
	}

	DynamicString Type::getFilename( const char* pAppendix, const char* pExtension ) const
	{
		DynamicString filename = m_fullName;
		for( uintreg i = 0u; i < filename.getLength(); ++i )
		{
			char c = filename[ i ];
			if( c == '.' )
			{
				c = '_';
			}
			else
			{
				c = ascii::toLower( c );
			}
			filename[ i ] = c;
		}

		if( !isStringEmpty( pAppendix ) )
		{
			filename += "_";
			filename += pAppendix;
		}

		if( !isStringEmpty( pExtension ) )
		{
			filename += ".";
			filename += pExtension;
		}

		return filename;
	}
}
