#include "type.h"

namespace conct
{
	Type::Type()
	{
		m_description = TypeDescription_Interface;
	}

	void Type::create( const std::string& namespaceVar, const std::string& name, TypeDescription description )
	{
		m_namespace		= namespaceVar;
		m_name			= name;
		m_description	= description;

		if( !m_namespace.empty() )
		{
			m_fullName = m_namespace + ".";
		}
		m_fullName += m_name;
	}
}
