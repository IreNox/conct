#include "conct_type.h"

#include "conct_array_type.h"
#include "conct_ascii.h"
#include "conct_crc16.h"

namespace conct
{
	Type::Type()
	{
		m_description = TypeDescription_Value;
	}

	void Type::create( const DynamicString& namespaceVar, const DynamicString& name, const DynamicString& cppName, TypeDescription description, ValueType valueType )
	{
		m_namespace		= namespaceVar;
		m_name			= name;
		m_cppName		= cppName;
		m_description	= description;
		m_valueType		= valueType;

		if( !m_namespace.isEmpty() )
		{
			m_fullName = m_namespace + ".";
		}
		m_fullName += m_name;

		switch( description )
		{
		case TypeDescription_Array:
			m_headerFilename = "conct_array_view.h"_s;
			break;

		case TypeDescription_Value:
			m_headerFilename = "conct_core.h"_s;
			break;

		default:
			m_headerFilename = getFilename( ""_s, "h"_s );
			break;
		}

		m_typeCrc = calculateCrc16( m_fullName.toConstCharPointer(), m_fullName.getLength() );
	}

	TypeCrc Type::getCrc() const
	{
		return m_typeCrc;
	}

	void Type::pushDependingType( const Type* pType )
	{
		m_dependingTypes.insert( pType );

		if( pType->getDescription() == TypeDescription_Array )
		{
			const ArrayType* pArrayType = static_cast< const ArrayType* >( pType );
			m_dependingTypes.insert( pArrayType->getBaseType() );
		}
	}

	DynamicString Type::getFilename( const DynamicString& appendix, const DynamicString& extension ) const
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

		if( !appendix.isEmpty() )
		{
			filename += "_"_s + appendix;
		}

		if( !extension.isEmpty() )
		{
			filename += "."_s + extension;
		}

		return filename;
	}
}
