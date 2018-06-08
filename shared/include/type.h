#pragma once

#include "base.h"

#include "conct_value.h"

#include <set>

namespace conct
{
	class TypeCollection;

	enum TypeDescription
	{
		TypeDescription_Value,
		TypeDescription_Interface,
		TypeDescription_Struct,
		TypeDescription_Enum,
		TypeDescription_Array,
	};

	class Type
	{
		friend class TypeCollection;

	public:

		typedef std::set< const Type* > TypeSet;

		const std::string&	getNamespace() const { return m_namespace; }
		const std::string&	getName() const { return m_name; }
		const std::string&	getCppName() const { return m_cppName; }
		const std::string&	getFullName() const { return m_fullName; }
		const std::string&	getHeaderFilename() const { return m_headerFilename; }

		TypeDescription		getDescription() const { return m_description; }
		ValueType			getValueType() const { return m_valueType; }

		const TypeSet&		getDependingTypes() const { return m_dependingTypes; }

		virtual TypeCrc		getCrc() const;

		std::string			getFilename( const std::string& appendix, const std::string& extension ) const;

	protected: // friend

							Type();
		void				create( const std::string& namespaceVar, const std::string& name, const std::string& cppName, TypeDescription description, ValueType valueType );

	protected:

		std::string			m_namespace;
		std::string			m_name;
		std::string			m_cppName;
		std::string			m_fullName;
		std::string			m_headerFilename;

		TypeDescription		m_description;
		ValueType			m_valueType;

		TypeSet				m_dependingTypes;

		void				pushDependingType( const Type* pType );
	};
}