#pragma once

#include "conct_value_type.h"
#include "conct_vector.h"

namespace conct
{
	class ArrayType;
	class DynamicString;
	class InterfaceType;
	class Path;
	class StructType;
	class Type;

	class TypeCollection
	{
	public:

												TypeCollection();
												~TypeCollection();

		bool									load( const Path& path );

		const Type*								addValueType( const DynamicString& namespaceVar, const DynamicString& name, const DynamicString& cppName, ValueType valueType );

		const Type*								findType( const DynamicString& fullName, const DynamicString& referenceNamespace );
		const Type*								findTypeByCrc( TypeCrc typeCrc );
		const InterfaceType*					findInterface( const DynamicString& fullName, const DynamicString& referenceNamespace );
		const InterfaceType*					findInterfaceByCrc( TypeCrc typeCrc );
		const StructType*						findStruct( const DynamicString& fullName, const DynamicString& referenceNamespace );

		const ArrayType*						makeArray( const Type* pBaseType );

		const Vector< Type* >&					getTypes() const { return m_types; }
		const Vector< InterfaceType* >&			getInterfaces() const { return m_interfaces; }
		const Vector< StructType* >&			getStruct() const { return m_structs; }

	private:

		Vector< Type* >							m_types;
		Vector< ArrayType* >					m_arrays;
		Vector< InterfaceType* >				m_interfaces;
		Vector< StructType* >					m_structs;

		bool									loadFiles( const Path& path );
	};
}