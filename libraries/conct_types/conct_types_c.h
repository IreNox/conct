#pragma once

#include "conct_core_c.h"

#ifdef __cplusplus
extern "C" {
#endif

// Types

enum conct_type_kind
{
	conct_type_kind_value,
	conct_type_kind_interface,
	conct_type_kind_struct,
	conct_type_kind_enum,
	conct_type_kind_array
};

struct conct_type;
typedef struct conct_type* conct_type_handle;

struct conct_interface_type;
typedef struct conct_interface_type* conct_interface_type_handle;

struct conct_struct_type;
typedef struct conct_struct_type* conct_struct_type_handle;

struct conct_array_type;
typedef struct conct_array_type* conct_array_type_handle;

struct conct_type_collection;
typedef struct conct_type_collection* conct_type_collection_handle;

// TypeCollection

CONCT_DLL conct_type_collection_handle		CONCT_CDECL conct_type_collection_create();
CONCT_DLL void								CONCT_CDECL conct_type_collection_destroy( conct_type_collection_handle handle );

CONCT_DLL bool								CONCT_CDECL conct_type_collection_load( conct_type_collection_handle handle, const char* pPath );

CONCT_DLL conct_type_handle					CONCT_CDECL conct_type_collection_add_value_type( conct_type_collection_handle handle, const char* pNamespace, const char* pName, const char* pCppName, conct_value_type valueType );

CONCT_DLL conct_type_handle					CONCT_CDECL conct_type_collection_find_type( conct_type_collection_handle handle, const char* pFullname, const char* pReferenceNamespace );
CONCT_DLL conct_type_handle					CONCT_CDECL conct_type_collection_find_type_by_crc( conct_type_collection_handle handle, conct_type_crc typeCrc );
CONCT_DLL conct_interface_type_handle		CONCT_CDECL conct_type_collection_find_interface( conct_type_collection_handle handle, const char* pFullname, const char* pReferenceNamespace );
CONCT_DLL conct_interface_type_handle		CONCT_CDECL conct_type_collection_find_interface_by_crc( conct_type_collection_handle handle, conct_type_crc typeCrc );
CONCT_DLL conct_struct_type_handle			CONCT_CDECL conct_type_collection_find_struct( conct_type_collection_handle handle, const char* pFullname, const char* pReferenceNamespace );
CONCT_DLL conct_struct_type_handle			CONCT_CDECL conct_type_collection_find_struct_by_crc( conct_type_collection_handle handle, conct_type_crc typeCrc );

CONCT_DLL conct_array_type_handle			CONCT_CDECL conct_type_collection_make_array( conct_type_collection_handle handle, conct_type_handle typeHandle );

CONCT_DLL int								CONCT_CDECL conct_type_collection_get_type_count( conct_type_collection_handle handle );
CONCT_DLL conct_type_handle					CONCT_CDECL conct_type_collection_find_type_by_index( conct_type_collection_handle handle, int index );

CONCT_DLL int								CONCT_CDECL conct_type_collection_get_interface_count( conct_type_collection_handle handle );
CONCT_DLL conct_interface_type_handle		CONCT_CDECL conct_type_collection_find_interface_by_index( conct_type_collection_handle handle, int index );

CONCT_DLL int								CONCT_CDECL conct_type_collection_get_struct_count( conct_type_collection_handle handle );
CONCT_DLL conct_struct_type_handle			CONCT_CDECL conct_type_collection_find_struct_by_index( conct_type_collection_handle handle, int index );

// Type

CONCT_DLL conct_interface_type_handle		CONCT_CDECL conct_type_cast_interface( conct_type_handle handle );
CONCT_DLL conct_struct_type_handle			CONCT_CDECL conct_type_cast_struct( conct_type_handle handle );
CONCT_DLL conct_array_type_handle			CONCT_CDECL conct_type_cast_array( conct_type_handle handle );

CONCT_DLL const char*						CONCT_CDECL conct_type_get_namespace( conct_type_handle handle );
CONCT_DLL const char*						CONCT_CDECL conct_type_get_name( conct_type_handle handle );
CONCT_DLL const char*						CONCT_CDECL conct_type_get_cpp_name( conct_type_handle handle );
CONCT_DLL const char*						CONCT_CDECL conct_type_get_full_name( conct_type_handle handle );
CONCT_DLL const char*						CONCT_CDECL conct_type_get_header_filename( conct_type_handle handle );

CONCT_DLL conct_type_kind					CONCT_CDECL conct_type_get_kind( conct_type_handle handle );
CONCT_DLL conct_value_type					CONCT_CDECL conct_type_get_value_type( conct_type_handle handle );
CONCT_DLL conct_type_crc					CONCT_CDECL conct_type_get_crc( conct_type_handle handle );

// InterfaceType

CONCT_DLL conct_type_handle					CONCT_CDECL conct_interface_type_cast_type( conct_interface_type_handle handle );

CONCT_DLL conct_interface_type_handle		CONCT_CDECL conct_interface_type_get_base_type( conct_interface_type_handle handle );

CONCT_DLL bool								CONCT_CDECL conct_interface_type_get_internal( conct_interface_type_handle handle );
CONCT_DLL bool								CONCT_CDECL conct_interface_type_get_singleton( conct_interface_type_handle handle );

CONCT_DLL int								CONCT_CDECL conct_interface_type_get_property_count( conct_interface_type_handle handle );
CONCT_DLL const char*						CONCT_CDECL conct_interface_type_get_property_name( conct_interface_type_handle handle, int index );
CONCT_DLL conct_type_handle					CONCT_CDECL conct_interface_type_get_property_type( conct_interface_type_handle handle, int index );
CONCT_DLL bool								CONCT_CDECL conct_interface_type_get_property_has_getter( conct_interface_type_handle handle, int index );
CONCT_DLL bool								CONCT_CDECL conct_interface_type_get_property_has_setter( conct_interface_type_handle handle, int index );

CONCT_DLL int								CONCT_CDECL conct_interface_type_get_function_count( conct_interface_type_handle handle );
CONCT_DLL const char*						CONCT_CDECL conct_interface_type_get_function_name( conct_interface_type_handle handle, int index );
CONCT_DLL conct_type_handle					CONCT_CDECL conct_interface_type_get_function_return_type( conct_interface_type_handle handle, int index );
CONCT_DLL int								CONCT_CDECL conct_interface_type_get_function_parameter_count( conct_interface_type_handle handle, int index );

CONCT_DLL const char*						CONCT_CDECL conct_interface_type_get_function_parameter_name( conct_interface_type_handle handle, int functionIndex, int parameterIndex );
CONCT_DLL conct_type_handle					CONCT_CDECL conct_interface_type_get_function_parameter_type( conct_interface_type_handle handle, int functionIndex, int parameterIndex );

// StructType

CONCT_DLL conct_type_handle					CONCT_CDECL conct_struct_type_cast_type( conct_struct_type_handle handle );

CONCT_DLL int								CONCT_CDECL conct_struct_type_get_field_count( conct_struct_type_handle handle );
CONCT_DLL const char*						CONCT_CDECL conct_struct_type_get_field_name( conct_struct_type_handle handle, int index );
CONCT_DLL conct_type_handle					CONCT_CDECL conct_struct_type_get_field_type( conct_struct_type_handle handle, int index );

// ArrayType

CONCT_DLL conct_type_handle					CONCT_CDECL conct_array_type_cast_type( conct_array_type_handle handle );

CONCT_DLL conct_type_handle					CONCT_CDECL conct_array_type_get_base_type( conct_array_type_handle handle );

#ifdef __cplusplus
}
#endif
