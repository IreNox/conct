#include "conct_types_c.h"

#include "conct_array_type.h"
#include "conct_core.h"
#include "conct_interface_type.h"
#include "conct_path.h"
#include "conct_struct_type.h"
#include "conct_type.h"
#include "conct_type_collection.h"

using namespace conct;

Type* fromHandle( conct_type_handle handle )
{
	return ( Type* )handle;
}

InterfaceType* fromHandle( conct_interface_type_handle handle )
{
	return ( InterfaceType* )handle;
}

StructType* fromHandle( conct_struct_type_handle handle )
{
	return ( StructType* )handle;
}

ArrayType* fromHandle( conct_array_type_handle handle )
{
	return ( ArrayType* )handle;
}

TypeCollection* fromHandle( conct_type_collection_handle handle )
{
	return ( TypeCollection* )handle;
}

conct_type_handle toHandle( const Type* pClass )
{
	return ( conct_type_handle )pClass;
}

conct_interface_type_handle toHandle( const InterfaceType* pClass )
{
	return ( conct_interface_type_handle )pClass;
}

conct_struct_type_handle toHandle( const StructType* pClass )
{
	return ( conct_struct_type_handle )pClass;
}

conct_array_type_handle toHandle( const ArrayType* pClass )
{
	return ( conct_array_type_handle )pClass;
}

conct_type_collection_handle toHandle( const TypeCollection* pClass )
{
	return ( conct_type_collection_handle )pClass;
}

// TypeCollection

CONCT_DLL conct_type_collection_handle CONCT_CDECL conct_type_collection_create()
{
	return toHandle( new TypeCollection() );
}

CONCT_DLL void CONCT_CDECL conct_type_collection_destroy( conct_type_collection_handle handle )
{
	delete fromHandle( handle );
}

CONCT_DLL bool CONCT_CDECL conct_type_collection_load( conct_type_collection_handle handle, const char* pPath )
{
	return fromHandle( handle )->load( Path( DynamicString( pPath ) ) );
}

CONCT_DLL conct_type_handle CONCT_CDECL conct_type_collection_add_value_type( conct_type_collection_handle handle, const char* pNamespace, const char* pName, const char* pCppName, conct_value_type valueType )
{
	return toHandle( fromHandle( handle )->addValueType( DynamicString( pNamespace ), DynamicString( pName ), DynamicString( pCppName ), ( ValueType )valueType ) );
}

CONCT_DLL conct_type_handle CONCT_CDECL conct_type_collection_find_type( conct_type_collection_handle handle, const char* pFullname, const char* pReferenceNamespace )
{
	return toHandle( fromHandle( handle )->findType( DynamicString( pFullname ), DynamicString( pReferenceNamespace ) ) );
}

CONCT_DLL conct_type_handle CONCT_CDECL conct_type_collection_find_type_by_crc( conct_type_collection_handle handle, conct_type_crc typeCrc )
{
	return toHandle( fromHandle( handle )->findTypeByCrc( typeCrc ) );
}

CONCT_DLL conct_interface_type_handle CONCT_CDECL conct_type_collection_find_interface( conct_type_collection_handle handle, const char* pFullname, const char* pReferenceNamespace )
{
	return toHandle( fromHandle( handle )->findInterface( DynamicString( pFullname ), DynamicString( pReferenceNamespace ) ) );
}

CONCT_DLL conct_interface_type_handle CONCT_CDECL conct_type_collection_find_interface_by_crc( conct_type_collection_handle handle, conct_type_crc typeCrc )
{
	return toHandle( fromHandle( handle )->findInterfaceByCrc( typeCrc ) );
}

CONCT_DLL conct_struct_type_handle CONCT_CDECL conct_type_collection_find_struct( conct_type_collection_handle handle, const char* pFullname, const char* pReferenceNamespace )
{
	return toHandle( fromHandle( handle )->findStruct( DynamicString( pFullname ), DynamicString( pReferenceNamespace ) ) );
}

CONCT_DLL conct_struct_type_handle CONCT_CDECL conct_type_collection_find_struct_by_crc( conct_type_collection_handle handle, conct_type_crc typeCrc )
{
	return toHandle( fromHandle( handle )->findStructByCrc( typeCrc ) );
}

CONCT_DLL conct_array_type_handle CONCT_CDECL conct_type_collection_make_array( conct_type_collection_handle handle, conct_type_handle typeHandle )
{
	return toHandle( fromHandle( handle )->makeArray( fromHandle( typeHandle ) ) );
}

CONCT_DLL int CONCT_CDECL conct_type_collection_get_type_count( conct_type_collection_handle handle )
{
	return int( fromHandle( handle )->getTypes().getLength() );
}

CONCT_DLL conct_type_handle CONCT_CDECL conct_type_collection_find_type_by_index( conct_type_collection_handle handle, int index )
{
	return toHandle( fromHandle( handle )->getTypes()[ uintreg( index ) ] );
}

CONCT_DLL int CONCT_CDECL conct_type_collection_get_interface_count( conct_type_collection_handle handle )
{
	return int( fromHandle( handle )->getInterfaces().getLength() );
}

CONCT_DLL conct_interface_type_handle CONCT_CDECL conct_type_collection_find_interface_by_index( conct_type_collection_handle handle, int index )
{
	return toHandle( fromHandle( handle )->getInterfaces()[ uintreg( index ) ] );
}

CONCT_DLL int CONCT_CDECL conct_type_collection_get_struct_count( conct_type_collection_handle handle )
{
	return int( fromHandle( handle )->getStruct().getLength() );
}

CONCT_DLL conct_struct_type_handle CONCT_CDECL conct_type_collection_find_struct_by_index( conct_type_collection_handle handle, int index )
{
	return toHandle( fromHandle( handle )->getStruct()[ uintreg( index ) ] );
}

// Type

CONCT_DLL conct_interface_type_handle CONCT_CDECL conct_type_cast_interface( conct_type_handle handle )
{
	if( fromHandle( handle )->getDescription() != TypeDescription_Interface )
	{
		return nullptr;
	}
	return ( conct_interface_type_handle )handle;
}

CONCT_DLL conct_struct_type_handle CONCT_CDECL conct_type_cast_struct( conct_type_handle handle )
{
	if( fromHandle( handle )->getDescription() != TypeDescription_Struct )
	{
		return nullptr;
	}
	return ( conct_struct_type_handle )handle;
}

CONCT_DLL conct_array_type_handle CONCT_CDECL conct_type_cast_array( conct_type_handle handle )
{
	if( fromHandle( handle )->getDescription() != TypeDescription_Array )
	{
		return nullptr;
	}
	return ( conct_array_type_handle )handle;
}

CONCT_DLL const char* CONCT_CDECL conct_type_get_namespace( conct_type_handle handle )
{
	return fromHandle( handle )->getNamespace().toConstCharPointer();
}

CONCT_DLL const char* CONCT_CDECL conct_type_get_name( conct_type_handle handle )
{
	return fromHandle( handle )->getName().toConstCharPointer();
}

CONCT_DLL const char* CONCT_CDECL conct_type_get_cpp_name( conct_type_handle handle )
{
	return fromHandle( handle )->getCppName().toConstCharPointer();
}

CONCT_DLL const char* CONCT_CDECL conct_type_get_full_name( conct_type_handle handle )
{
	return fromHandle( handle )->getFullName().toConstCharPointer();
}

CONCT_DLL const char* CONCT_CDECL conct_type_get_header_filename( conct_type_handle handle )
{
	return fromHandle( handle )->getHeaderFilename().toConstCharPointer();
}

CONCT_DLL conct_type_description CONCT_CDECL conct_type_get_description( conct_type_handle handle )
{
	return ( conct_type_description )fromHandle( handle )->getDescription();
}

CONCT_DLL conct_value_type CONCT_CDECL conct_type_get_value_type( conct_type_handle handle )
{
	return ( conct_value_type )fromHandle( handle )->getValueType();
}

CONCT_DLL conct_type_crc CONCT_CDECL conct_type_get_crc( conct_type_handle handle )
{
	return fromHandle( handle )->getCrc();
}

// InterfaceType

CONCT_DLL conct_type_handle CONCT_CDECL conct_interface_type_cast_type( conct_interface_type_handle handle )
{
	return ( conct_type_handle )handle;
}

CONCT_DLL conct_interface_type_handle CONCT_CDECL conct_interface_type_get_base_type( conct_interface_type_handle handle )
{
	return toHandle( fromHandle( handle )->getBaseType() );
}

CONCT_DLL bool CONCT_CDECL conct_interface_type_get_internal( conct_interface_type_handle handle )
{
	return fromHandle( handle )->getInternal();
}

CONCT_DLL bool CONCT_CDECL conct_interface_type_get_singleton( conct_interface_type_handle handle )
{
	return fromHandle( handle )->getSingleton();
}

CONCT_DLL int CONCT_CDECL conct_interface_type_get_property_count( conct_interface_type_handle handle )
{
	return int( fromHandle( handle )->getProperties().getLength() );
}

CONCT_DLL const char* CONCT_CDECL conct_interface_type_get_property_name( conct_interface_type_handle handle, int index )
{
	return fromHandle( handle )->getProperties()[ uintreg( index ) ].name.toConstCharPointer();
}

CONCT_DLL conct_type_handle CONCT_CDECL conct_interface_type_get_property_type( conct_interface_type_handle handle, int index )
{
	return toHandle( fromHandle( handle )->getProperties()[ uintreg( index ) ].pType );
}

CONCT_DLL bool CONCT_CDECL conct_interface_type_get_property_has_getter( conct_interface_type_handle handle, int index )
{
	return fromHandle( handle )->getProperties()[ uintreg( index ) ].hasGetter;
}

CONCT_DLL bool CONCT_CDECL conct_interface_type_get_property_has_setter( conct_interface_type_handle handle, int index )
{
	return fromHandle( handle )->getProperties()[ uintreg( index ) ].hasSetter;
}

CONCT_DLL int CONCT_CDECL conct_interface_type_get_function_count( conct_interface_type_handle handle )
{
	return int( fromHandle( handle )->getFunctions().getLength() );
}

CONCT_DLL const char* CONCT_CDECL conct_interface_type_get_function_name( conct_interface_type_handle handle, int index )
{
	return fromHandle( handle )->getFunctions()[ uintreg( index ) ].name.toConstCharPointer();
}

CONCT_DLL conct_type_handle CONCT_CDECL conct_interface_type_get_function_return_type( conct_interface_type_handle handle, int index )
{
	return toHandle( fromHandle( handle )->getFunctions()[ uintreg( index ) ].pReturnType );
}

CONCT_DLL int CONCT_CDECL conct_interface_type_get_function_parameter_count( conct_interface_type_handle handle, int index )
{
	return int( fromHandle( handle )->getFunctions()[ uintreg( index ) ].parameters.getLength() );
}

CONCT_DLL const char* CONCT_CDECL conct_interface_type_get_function_parameter_name( conct_interface_type_handle handle, int functionIndex, int parameterIndex )
{
	return fromHandle( handle )->getFunctions()[ uintreg( functionIndex ) ].parameters[ uintreg( parameterIndex ) ].name.toConstCharPointer();
}

CONCT_DLL conct_type_handle CONCT_CDECL conct_interface_type_get_function_parameter_type( conct_interface_type_handle handle, int functionIndex, int parameterIndex )
{
	return toHandle( fromHandle( handle )->getFunctions()[ uintreg( functionIndex ) ].parameters[ uintreg( parameterIndex ) ].pType );
}

// StructType

CONCT_DLL conct_type_handle CONCT_CDECL conct_struct_type_cast_type( conct_struct_type_handle handle )
{
	return ( conct_type_handle )handle;
}

CONCT_DLL int CONCT_CDECL conct_struct_type_get_field_count( conct_struct_type_handle handle )
{
	return int( fromHandle( handle )->getFields().getLength() );
}

CONCT_DLL const char* CONCT_CDECL conct_struct_type_get_field_name( conct_struct_type_handle handle, int index )
{
	return fromHandle( handle )->getFields()[ index ].name.toConstCharPointer();
}

CONCT_DLL conct_type_handle CONCT_CDECL conct_struct_type_get_field_type( conct_struct_type_handle handle, int index )
{
	return toHandle( fromHandle( handle )->getFields()[ index ].pType );
}

// ArrayType

CONCT_DLL conct_type_handle CONCT_CDECL conct_array_type_cast_type( conct_array_type_handle handle )
{
	return ( conct_type_handle )handle;
}

CONCT_DLL conct_type_handle CONCT_CDECL conct_array_type_get_base_type( conct_array_type_handle handle )
{
	return toHandle( fromHandle( handle )->getBaseType() );
}
