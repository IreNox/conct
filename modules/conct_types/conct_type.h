#pragma once

#include "conct_dynamic_string.h"
#include "conct_path.h"
#include "conct_unsorted_set.h"
#include "conct_value_type.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace conct
{
	class TypeCollection;

	enum TypeKind
	{
		TypeKind_Value,
		TypeKind_Interface,
		TypeKind_Struct,
		TypeKind_Enum,
		TypeKind_Array
	};

	class Type
	{
		friend class TypeCollection;

	public:

		typedef UnsortedSet< const Type* > TypeSet;

		virtual					~Type() { }

		const Path&				getFileName() const { return m_fileName; }

		const DynamicString&	getNamespace() const { return m_namespace; }
		const DynamicString&	getName() const { return m_name; }
		const DynamicString&	getCppName() const { return m_cppName; }
		const DynamicString&	getFullName() const { return m_fullName; }
		const DynamicString&	getHeaderFilename() const { return m_headerFilename; }

		TypeKind				getKind() const { return m_kind; }
		ValueType				getValueType() const { return m_valueType; }

		const TypeSet&			getDependingTypes() const { return m_dependingTypes; }

		TypeCrc					getCrc() const;
		bool					getInternal() const { return m_internal; }

		DynamicString			getFilename( const char* pAppendix, const char* pExtension ) const;

	protected: // friend

								Type();
		void					create( const Path& fileName, const DynamicString& namespaceVar, const DynamicString& name, const DynamicString& cppName, TypeKind kind, ValueType valueType, bool internal );

		bool					loadInternal( const tinyxml2::XMLElement* pRootNode );

	protected:

		Path					m_fileName;

		DynamicString			m_namespace;
		DynamicString			m_name;
		DynamicString			m_cppName;
		DynamicString			m_fullName;
		DynamicString			m_headerFilename;

		TypeKind				m_kind;
		ValueType				m_valueType;
		TypeCrc					m_typeCrc;
		bool					m_internal;

		TypeSet					m_dependingTypes;

		void					pushDependingType( const Type* pType );
	};
}