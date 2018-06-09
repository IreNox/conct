#pragma once

#include "base.h"

#include "conct_value_type.h"

namespace conct
{
	class ArrayType;
	class InterfaceType;
	class StructType;
	class Type;

	class TypeCollection
	{
	public:

		TypeCollection();
		~TypeCollection();

		bool									load( const std::string& path );

		const Type*								addValueType( const std::string& namespaceVar, const std::string& name, const std::string& cppName, ValueType valueType );

		const Type*								findType( const std::string& fullName, const std::string& referenceNamespace );
		const InterfaceType*					findInterface( const std::string& fullName, const std::string& referenceNamespace );
		const StructType*						findStruct( const std::string& fullName, const std::string& referenceNamespace );

		const ArrayType*						makeArray( const Type* pBaseType );

		const std::vector< Type* >&				getTypes() const { return m_types; }
		const std::vector< InterfaceType* >&	getInterfaces() const { return m_interfaces; }
		const std::vector< StructType* >&		getStruct() const { return m_structs; }

	private:

		std::vector< Type* >					m_types;
		std::vector< ArrayType* >				m_arrays;
		std::vector< InterfaceType* >			m_interfaces;
		std::vector< StructType* >				m_structs;

		bool									loadFiles( const std::string& path );
	};
}