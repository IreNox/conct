#pragma once

#include "base.h"

namespace conct
{
	class Array;
	class Interface;
	class Type;

	class TypeCollection
	{
	public:

									TypeCollection();
									~TypeCollection();

		bool						load( const std::string& path );

		const Type*					addValueType( const std::string& namespaceVar, const std::string& name, const std::string& cppName );

		const Type*					findType( const std::string& fullName, const std::string& referenceNamespace );
		const Interface*			findInterface( const std::string& fullName, const std::string& referenceNamespace );

		const Array*				makeArray( const Type* pBaseType );

	private:

		std::vector< Type* >		m_types;
		std::vector< Array* >		m_arrays;
		std::vector< Interface* >	m_interfaces;

		bool						loadFiles( const std::string& path );
	};
}