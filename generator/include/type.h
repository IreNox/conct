#pragma once

#include "base.h"

namespace conct
{
	class TypeCollection;

	enum TypeDescription
	{
		TypeDescription_Value,
		TypeDescription_Interface,
		TypeDescription_Enum,
		TypeDescription_Array,
	};

	class Type
	{
		friend class TypeCollection;

	public:

		const std::string&	getNamespace() const { return m_namespace; }
		const std::string&	getName() const { return m_name; }
		const std::string&	getCppName() const { return m_cppName; }
		const std::string&	getFullName() const { return m_fullName; }
		TypeDescription		getDescription() const { return m_description; }

	protected: // friend

							Type();
		void				create( const std::string& namespaceVar, const std::string& name, const std::string& cppName, TypeDescription description );

	protected:

		std::string			m_namespace;
		std::string			m_name;
		std::string			m_cppName;
		std::string			m_fullName;
		TypeDescription		m_description;
	};
}