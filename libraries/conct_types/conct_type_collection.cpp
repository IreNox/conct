#include "conct_type_collection.h"

#include "conct_array_type.h"
#include "conct_directory_iterator.h"
#include "conct_enum_type.h"
#include "conct_filesystem.h"
#include "conct_interface_type.h"
#include "conct_path.h"
#include "conct_struct_type.h"
#include "conct_trace.h"
#include "conct_type.h"
#include "conct_xml_helper.h"

#include <tinyxml2/tinyxml2.h>

namespace conct
{
	TypeCollection::TypeCollection()
	{
		addValueType( ""_s, "Void"_s, "void"_s, ValueType_Void );
		addValueType( ""_s, "Boolean"_s, "bool"_s, ValueType_Boolean );
		addValueType( ""_s, "Integer"_s, "sint32"_s, ValueType_Integer );
		addValueType( ""_s, "Unsigned"_s, "uint32"_s, ValueType_Unsigned );
		addValueType( ""_s, "String"_s, "const char*"_s, ValueType_String );
		addValueType( ""_s, "PercentValue"_s, "PercentValue"_s, ValueType_PercentValue );
		addValueType( ""_s, "DeviceId"_s, "DeviceId"_s, ValueType_DeviceId );
		addValueType( ""_s, "TypeCrc"_s, "TypeCrc"_s, ValueType_TypeCrc );
		addValueType( ""_s, "InstanceId"_s, "InstanceId"_s, ValueType_InstanceId );
	}

	TypeCollection::~TypeCollection()
	{
		while( !m_types.isEmpty() )
		{
			delete( m_types.getBack() );
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
				trace::write( "Error: Failed to load interface from '"_s + pInterface->getFileName().getGenericPath() + "'." + "\n" );
				return false;
			}
		}

		for( StructType* pStruct : m_structs )
		{
			if( !pStruct->load( *this ) )
			{
				trace::write( "Error: Failed to load struct from '"_s + pStruct->getFileName().getGenericPath() + "'." + "\n" );
				return false;
			}
		}

		for( EnumType* pEnum : m_enums )
		{
			if( !pEnum->load( *this ) )
			{
				trace::write( "Error: Failed to load enum from '"_s + pEnum->getFileName().getGenericPath() + "'." + "\n" );
				return false;
			}
		}

		return true;
	}

	const Type* TypeCollection::addValueType( const DynamicString& namespaceVar, const DynamicString& name, const DynamicString& cppName, ValueType valueType )
	{
		Type* pType = new Type();
		pType->create( Path(), namespaceVar, name, cppName, TypeKind_Value, valueType, true );

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
		if( pType != nullptr && pType->getKind() == TypeKind_Interface )
		{
			return static_cast< const InterfaceType* >( pType );
		}

		return nullptr;
	}

	const InterfaceType* TypeCollection::findInterfaceByCrc( TypeCrc typeCrc )
	{
		const Type* pType = findTypeByCrc( typeCrc );
		if( pType != nullptr && pType->getKind() == TypeKind_Interface )
		{
			return static_cast< const InterfaceType* >( pType );
		}

		return nullptr;
	}

	const StructType* TypeCollection::findStruct( const DynamicString& fullName, const DynamicString& referenceNamespace )
	{
		const Type* pType = findType( fullName, referenceNamespace );
		if( pType != nullptr && pType->getKind() == TypeKind_Struct )
		{
			return static_cast< const StructType* >( pType );
		}

		return nullptr;
	}

	const StructType* TypeCollection::findStructByCrc( TypeCrc typeCrc )
	{
		const Type* pType = findTypeByCrc( typeCrc );
		if( pType != nullptr && pType->getKind() == TypeKind_Struct )
		{
			return static_cast<const StructType*>(pType);
		}

		return nullptr;
	}

	const EnumType* TypeCollection::findEnum( const DynamicString& fullName, const DynamicString& referenceNamespace )
	{
		const Type* pType = findType( fullName, referenceNamespace );
		if( pType != nullptr && pType->getKind() == TypeKind_Enum )
		{
			return static_cast<const EnumType*>(pType);
		}

		return nullptr;

	}

	const EnumType* TypeCollection::findEnumByCrc( TypeCrc typeCrc )
	{
		const Type* pType = findTypeByCrc( typeCrc );
		if( pType != nullptr && pType->getKind() == TypeKind_Enum )
		{
			return static_cast<const EnumType*>(pType);
		}

		return nullptr;
	}

	const ArrayType* TypeCollection::makeArray( const Type* pBaseType )
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
			if( !loadDocument( document, currentPath ) )
			{
				return false;
			}

			tinyxml2::XMLElement* pRootNode = document.RootElement();
			if( pRootNode == nullptr )
			{
				trace::write( "Error: Failed to find root node in '"_s + currentPath.getGenericPath() + "'." + "\n" );
				return false;
			}

			DynamicString namespaceVar;
			DynamicString name;
			if( !loadStringValue( namespaceVar, pRootNode, "namespace" ) ||
				!loadStringValue( name, pRootNode, "name" ) )
			{
				return false;
			}

			const DynamicString rootNodeName( pRootNode->Name() );
			if( rootNodeName == "interface" )
			{
				InterfaceType* pInterface = new InterfaceType();
				pInterface->create( currentPath, namespaceVar, name );

				m_types.pushBack( pInterface );
				m_interfaces.pushBack( pInterface );
			}
			else if( rootNodeName == "struct" )
			{
				StructType* pStruct = new StructType();
				pStruct->create( currentPath, namespaceVar, name );

				m_types.pushBack( pStruct );
				m_structs.pushBack( pStruct );
			}
			else if( rootNodeName == "enum" )
			{
				EnumType* pEnum = new EnumType();
				pEnum->create( currentPath, namespaceVar, name );

				m_types.pushBack( pEnum );
				m_enums.pushBack( pEnum );
			}
			else
			{
				trace::write( "Error: Unknown type '"_s + rootNodeName + "' in '" + currentPath.getGenericPath() + "'." + "\n" );
				return false;
			}
		}

		return true;
	}
}
