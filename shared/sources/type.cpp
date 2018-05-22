#include "type.h"

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
	}

	TypeCrc Type::getCrc() const
	{
		return calculateCrc16( m_fullName.data(), m_fullName.size() );
	}
}
