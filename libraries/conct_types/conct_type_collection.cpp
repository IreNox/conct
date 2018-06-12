#include "conct_type_collection.h"

#include "conct_array_type.h"
#include "conct_directory_iterator.h"
#include "conct_filesystem.h"
#include "conct_interface_type.h"
#include "conct_path.h"
#include "conct_struct_type.h"
#include "conct_type.h"
#include "conct_xml_helper.h"

#include <tinyxml2/tinyxml2.h>

#include <iostream>

namespace conct
{
	TypeCollection::TypeCollection()
	{
		addValueType( ""_s, "Boolean"_s, "bool"_s, ValueType_Boolean );
		addValueType( ""_s, "Integer"_s, "sint32"_s, ValueType_Integer );
		addValueType( ""_s, "Unsigned"_s, "uin32"_s, ValueType_Unsigned );
		addValueType( ""_s, "String"_s, "const char*"_s, ValueType_String );
		addValueType( ""_s, "PercentValue"_s, "PercentValue"_s, ValueType_PercentValue );
		addValueType( ""_s, "Guid"_s, "Guid"_s, ValueType_Guid );
		addValueType( ""_s, "DeviceId"_s, "DeviceId"_s, ValueType_DeviceId );
		addValueType( ""_s, "TypeCrc"_s, "TypeCrc"_s, ValueType_TypeCrc );
		addValueType( ""_s, "Instance"_s, "Instance"_s, ValueType_Instance );
	}

	TypeCollection::~TypeCollection()
	{
		while( !m_types.isEmpty() )
		{
			delete( m_types.getLast() );
			m_types.popBack();
		}
	}

	bool TypeCollection::load( const Path& path )
	{
		if( !loadFiles( path ) )
		{
			return false;
		}

		for( InterfaceType* pInterface : m_interfaces )
		{
			if( !pInterface->load( *this ) )
			{
				std::cout << "Error: Failed to load interface from '" << pInterface->getFileName().getGenericPath() << "'." << std::endl;
				return false;
			}
		}

		for( StructType* pStruct : m_structs )
		{
			if( !pStruct->load( *this ) )
			{
				std::cout << "Error: Failed to load struct from '" << pStruct->getFileName().getGenericPath() << "'." << std::endl;
				return false;
			}
		}

		return true;
	}

	const Type* TypeCollection::addValueType( const DynamicString& namespaceVar, const DynamicString& name, const DynamicString& cppName, ValueType valueType )
	{
		Type* pType = new Type();
		pType->create( namespaceVar, name, cppName, TypeDescription_Value, valueType );

		m_types.pushBack( pType );
		return pType;
	}

	const Type* TypeCollection::findType( const DynamicString& fullName, const DynamicString& referenceNamespace )
	{
		DynamicString namespaceVar;
		DynamicString name = fullName;

		const size_t lastDot = fullName.lastIndexOf( '.' );
		if( lastDot != InvalidStringIndex )
		{
			namespaceVar = fullName.subString( 0u, lastDot );
			name = fullName.subString( lastDot + 1u );
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

	const Type* TypeCollection::findTypeByCrc( TypeCrc typeCrc )
	{
		for( const Type* pType : m_types )
		{
			if( pType->getCrc() == typeCrc )
			{
				return pType;
			}
		}

		return nullptr;
	}

	const InterfaceType* TypeCollection::findInterface( const DynamicString& fullName, const DynamicString& referenceNamespace )
	{
		const Type* pType = findType( fullName, referenceNamespace );
		if( pType != nullptr && pType->getDescription() == TypeDescription_Interface )
		{
			return static_cast< const InterfaceType* >( pType );
		}

		return nullptr;
	}

	const InterfaceType* TypeCollection::findInterfaceByCrc( TypeCrc typeCrc )
	{
		const Type* pType = findTypeByCrc( typeCrc );
		if( pType != nullptr && pType->getDescription() == TypeDescription_Interface )
		{
			return static_cast< const InterfaceType* >( pType );
		}

		return nullptr;
	}

	const conct::StructType* TypeCollection::findStruct( const DynamicString& fullName, const DynamicString& referenceNamespace )
	{
		const Type* pType = findType( fullName, referenceNamespace );
		if( pType != nullptr && pType->getDescription() == TypeDescription_Struct )
		{
			return static_cast< const StructType* >( pType );
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

		m_types.pushBack( pArray );
		m_arrays.pushBack( pArray );

		return pArray;
	}

	bool TypeCollection::loadFiles( const Path& path )
	{
		DirectoryIterator iterator( path );
		while( iterator.next() )
		{
			const Path& currentPath = iterator.getCurrent();

			if( filesystem::isDirectory( currentPath ) )
			{
				if( !loadFiles( currentPath ) )
				{
					return false;
				}

				continue;
			}

			tinyxml2::XMLDocument document;
			if( document.LoadFile( currentPath.getNativePath().toConstCharPointer() ) != tinyxml2::XML_SUCCESS )
			{
				std::cout << "Error: Failed to load XML from '" << currentPath.getGenericPath() << "'. Message: " << document.ErrorStr() << std::endl;
				return false;
			}

			tinyxml2::XMLElement* pRootNode = document.RootElement();
			if( pRootNode == nullptr )
			{
				std::cout << "Error: Failed to find root node in '" << currentPath.getGenericPath() << "'." << std::endl;
				return false;
			}

			const DynamicString rootNodeName( pRootNode->Name() );
			if( rootNodeName == "interface" )
			{
				InterfaceType* pInterface = new InterfaceType();

				DynamicString namespaceVar;
				DynamicString name;
				if( !loadStringValue( namespaceVar, pRootNode, "namespace" ) ||
					!loadStringValue( name, pRootNode, "name" ) )
				{
					return false;
				}
				pInterface->create( currentPath, namespaceVar, name );

				m_types.pushBack( pInterface );
				m_interfaces.pushBack( pInterface );
			}
			else if( rootNodeName == "struct" )
			{
				StructType* pStruct = new StructType();

				DynamicString namespaceVar;
				DynamicString name;
				if( !loadStringValue( namespaceVar, pRootNode, "namespace" ) ||
					!loadStringValue( name, pRootNode, "name" ) )
				{
					return false;
				}
				pStruct->create( currentPath, namespaceVar, name );

				m_types.pushBack( pStruct );
				m_structs.pushBack( pStruct );
			}
			else if( rootNodeName == "enum" )
			{
				std::cout << "Info: Enum in '" << currentPath.getGenericPath() << "' is currently not supported." << std::endl;
			}
			else
			{
				std::cout << "Error: Unknown type '" << rootNodeName << "' in '" << currentPath.getGenericPath() << "'." << std::endl;
				return false;
			}
		}

		return true;
	}
}
