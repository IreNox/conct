#pragma once

#include "conct_value_type.h"

#include <tiki/tiki_dynamic_array.h>

namespace tiki
{
	class DynamicString;
	class Path;
}

namespace conct
{
	class ArrayType;
	class EnumType;
	class InterfaceType;
	class StructType;
	class Type;

	class TypeCollection
	{
	public:

										TypeCollection();
										~TypeCollection();

		bool							load( const Path& path );

		const Type*						addValueType( const DynamicString& namespaceVar, const DynamicString& name, const DynamicString& cppName, ValueType valueType );

		const Type*						findType( const DynamicString& fullName, const DynamicString& referenceNamespace );
		const Type*						findTypeByCrc( TypeCrc typeCrc );
		const InterfaceType*			findInterface( const DynamicString& fullName, const DynamicString& referenceNamespace );
		const InterfaceType*			findInterfaceByCrc( TypeCrc typeCrc );
		const StructType*				findStruct( const DynamicString& fullName, const DynamicString& referenceNamespace );
		const StructType*				findStructByCrc( TypeCrc typeCrc );
		const EnumType*					findEnum( const DynamicString& fullName, const DynamicString& referenceNamespace );
		const EnumType*					findEnumByCrc( TypeCrc typeCrc );

		const ArrayType*				makeArray( const Type* pBaseType );

		ArrayView< Type* >				getTypes() const { return m_types; }
		ArrayView< InterfaceType* >		getInterfaces() const { return m_interfaces; }
		ArrayView< StructType* >		getStruct() const { return m_structs; }

	private:

		DynamicArray< Type* >			m_types;
		DynamicArray< ArrayType* >		m_arrays;
		DynamicArray< InterfaceType* >	m_interfaces;
		DynamicArray< StructType* >		m_structs;
		DynamicArray< EnumType* >		m_enums;

		bool							loadFiles( const Path& path );
	};
}