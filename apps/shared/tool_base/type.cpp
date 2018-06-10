#include "type.h"

#include "array_type.h"

#include "conct_ascii.h"

namespace conct
{
	Type::Type()
	{
		m_description = TypeDescription_Value;
	}

	void Type::create( const std::string& namespaceVar, const std::string& name, const std::string& cppName, TypeDescription description, ValueType valueType )
	{
		m_namespace		= namespaceVar;
		m_name			= name;
		m_cppName		= cppName;
		m_description	= description;
		m_valueType		= valueType;

		if( !m_namespace.empty() )
		{
			m_fullName = m_namespace + ".";
		}
		m_fullName += m_name;

		switch( description )
		{
		case TypeDescription_Array:
			m_headerFilename = "conct_array_view.h";
			break;

		case TypeDescription_Value:
			m_headerFilename = "conct_core.h";
			break;

		default:
			m_headerFilename = getFilename( "", "h" );
			break;
		}

		m_typeCrc = calculateCrc16( m_fullName.data(), m_fullName.size() );
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

	std::string Type::getFilename( const std::string& appendix, const std::string& extension ) const
	{
		std::string filename = m_fullName;
		for( uintreg i = 0u; i < filename.size(); ++i )
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

		if( !appendix.empty() )
		{
			filename += "_" + appendix;
		}

		if( !extension.empty() )
		{
			filename += "." + extension;
		}

		return filename;
	}
}
