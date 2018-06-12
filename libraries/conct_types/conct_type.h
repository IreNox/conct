#pragma once

#include "conct_dynamic_string.h"
#include "conct_unsorted_set.h"
#include "conct_value_type.h"

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

		typedef UnsortedSet< const Type* > TypeSet;

		const DynamicString&	getNamespace() const { return m_namespace; }
		const DynamicString&	getName() const { return m_name; }
		const DynamicString&	getCppName() const { return m_cppName; }
		const DynamicString&	getFullName() const { return m_fullName; }
		const DynamicString&	getHeaderFilename() const { return m_headerFilename; }

		TypeDescription			getDescription() const { return m_description; }
		ValueType				getValueType() const { return m_valueType; }

		const TypeSet&			getDependingTypes() const { return m_dependingTypes; }

		virtual TypeCrc			getCrc() const;

		DynamicString			getFilename( const char* pAppendix, const char* pExtension ) const;

	protected: // friend

								Type();
		void					create( const DynamicString& namespaceVar, const DynamicString& name, const DynamicString& cppName, TypeDescription description, ValueType valueType );

	protected:

		DynamicString			m_namespace;
		DynamicString			m_name;
		DynamicString			m_cppName;
		DynamicString			m_fullName;
		DynamicString			m_headerFilename;

		TypeDescription			m_description;
		ValueType				m_valueType;
		TypeCrc					m_typeCrc;

		TypeSet					m_dependingTypes;

		void					pushDependingType( const Type* pType );
	};
}