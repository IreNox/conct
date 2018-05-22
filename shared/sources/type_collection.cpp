#include "type_collection.h"

#include "array_type.h"
#include "interface_type.h"
#include "type.h"
#include "xml_helper.h"

#include <third_party/tinyxml2.h>

#include <filesystem>
#include <iostream>

namespace std
{
	namespace filesystem = std::experimental::filesystem;
}

namespace conct
{
	TypeCollection::TypeCollection()
	{
		addValueType( "", "Boolean", "bool", ValueType_Boolean );
		addValueType( "", "Integer", "int", ValueType_Integer );
		addValueType( "", "Unsigned", "unsigned", ValueType_Unsigned );
		addValueType( "", "String", "std::string", ValueType_String );
		addValueType( "", "PercentValue", "PercentValue", ValueType_PercentValue );
		addValueType( "", "Guid", "Guid", ValueType_Guid );
		addValueType( "", "TypeDescription", "error", ValueType_Integer ); // fake enum
	}

	TypeCollection::~TypeCollection()
	{
		while( !m_types.empty() )
		{
			delete( m_types.back() );
			m_types.pop_back();
		}
	}

	bool TypeCollection::load( const std::string& path )
	{
		if( !loadFiles( path ) )
		{
			return false;
		}

		for( InterfaceType* pInterface : m_interfaces )
		{
			if( !pInterface->load( *this ) )
			{
				std::cout << "Error: Failed to load interface from '" << pInterface->getFileName() << "'." << std::endl;
				return false;
			}
		}

		return true;
	}

	const Type* TypeCollection::addValueType( const std::string& namespaceVar, const std::string& name, const std::string& cppName, ValueType valueType )
	{
		Type* pType = new Type();
		pType->create( namespaceVar, name, cppName, TypeDescription_Value, valueType );

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

	const InterfaceType* TypeCollection::findInterface( const std::string& fullName, const std::string& referenceNamespace )
	{
		const Type* pType = findType( fullName, referenceNamespace );
		if( pType != nullptr && pType->getDescription() == TypeDescription_Interface )
		{
			return static_cast< const InterfaceType* >( pType );
		}

		return nullptr;
	}

	const conct::ArrayType* TypeCollection::makeArray( const Type* pBaseType )
	{
		for( const ArrayType* pArray : m_arrays )
		{
			if( pArray->getBaseType() == pBaseType )
			{
				return pArray;
			}
		}

		ArrayType* pArray = new ArrayType();
		pArray->create( pBaseType );

		m_types.push_back( pArray );
		m_arrays.push_back( pArray );

		return pArray;
	}

	bool TypeCollection::loadFiles( const std::string& path )
	{
		std::filesystem::directory_iterator iterator( path );
		for( const std::filesystem::directory_entry& entry : iterator )
		{
			if( std::filesystem::is_directory( entry.path() ) )
			{
				if( !loadFiles( entry.path().generic_string() ) )
				{
					return false;
				}

				continue;
			}

			const std::string fileName = entry.path().generic_string();

			tinyxml2::XMLDocument document;
			if( document.LoadFile( fileName.c_str() ) != tinyxml2::XML_SUCCESS )
			{
				std::cout << "Error: Failed to load XML from '" << fileName << "'. Message: " << document.ErrorStr() << std::endl;
				return false;
			}

			tinyxml2::XMLElement* pRootNode = document.RootElement();
			if( pRootNode == nullptr )
			{
				std::cout << "Error: Failed to find root node in '" << fileName << "'." << std::endl;
				return false;
			}

			const std::string rootNodeName = pRootNode->Name();
			if( rootNodeName == "interface" )
			{
				InterfaceType* pInterface = new InterfaceType();

				std::string namespaceVar;
				std::string name;
				if( !loadStringValue( namespaceVar, pRootNode, "namespace" ) ||
					!loadStringValue( name, pRootNode, "name" ) )
				{
					return false;
				}
				pInterface->create( fileName, namespaceVar, name );

				m_types.push_back( pInterface );
				m_interfaces.push_back( pInterface );
			}
			else if( rootNodeName == "enum" )
			{
				std::cout << "Info: Enum in '" << fileName << "' is currently not supported." << std::endl;
			}
			else
			{
				std::cout << "Error: Unknown type '" << rootNodeName << "' in '" << fileName << "'." << std::endl;
				return false;
			}
		}

		return true;
	}
}
