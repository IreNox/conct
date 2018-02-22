#include "type_collection.h"

namespace conct
{
	TypeCollection::TypeCollection()
	{
	}

	TypeCollection::~TypeCollection()
	{
		while( !m_types.empty() )
		{
			delete( m_types.back() );
			m_types.pop_back();
		}
	}

	const Type* TypeCollection::addValueType( const std::string& namespaceVar, const std::string& name )
	{
		Type* pType = new Type();
		pType->create( namespaceVar, name, TypeDescription_Value );

		m_types.push_back( pType );
		return pType;
	}

	const Type* TypeCollection::findType( const std::string& fullName, const std::string& referenceNamespace )
	{
		std::string namespaceVar;
		std::string name = fullName;

		const size_t lastDot = fullName.find_last_of( '.' );
		if( lastDot != std::string::npos )
		{
			namespaceVar = fullName.substr( 0u, lastDot );
			name = fullName.substr( lastDot + 1u );
		}

		for( const Type* pType : m_types )
		{
			if( pType->getName() == name && ( pType->getNamespace() == namespaceVar || pType->getNamespace() == referenceNamespace ) )
			{
				return pType;
			}
		}

		return nullptr;
	}

	const Type* TypeCollection::addOrFindType( const std::string& fullName, const std::string& referenceNamespace )
	{
		return nullptr;
	}

	const Interface* TypeCollection::findInterface( const std::string& fullName, const std::string& referenceNamespace )
	{
		const Type* pType = findType( fullName, referenceNamespace );
		if( pType != nullptr && pType->getDescription() == TypeDescription_Interface )
		{
			return static_cast< const Interface* >( pType );
		}

		return nullptr;
	}

	const Interface* TypeCollection::addOrFindInterface( const std::string& fullName, const std::string& referenceNamespace )
	{
		return nullptr;
	}
}
