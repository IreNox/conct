#pragma once

#include "type.h"
#include "interface.h"

namespace conct
{
	class TypeCollection
	{
	public:

								TypeCollection();
								~TypeCollection();

		const Type*				addValueType( const std::string& namespaceVar, const std::string& name );

		const Type*				findType( const std::string& fullName, const std::string& referenceNamespace );
		const Type*				addOrFindType( const std::string& fullName, const std::string& referenceNamespace );

		const Interface*		findInterface( const std::string& fullName, const std::string& referenceNamespace );
		const Interface*		addOrFindInterface( const std::string& fullName, const std::string& referenceNamespace );

	private:

		std::vector< Type* >	m_types;
	};
}