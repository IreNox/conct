#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Types

enum conct_value_type
{
	conct_value_type_void,
	conct_value_type_boolean,
	conct_value_type_integer,
	conct_value_type_unsigned,
	conct_value_type_percent_value,
	conct_value_type_guid,
	conct_value_type_device_id,
	conct_value_type_instance,
	conct_value_type_type_crc,
	conct_value_type_string,
	conct_value_type_struct,
	conct_value_type_array
};

enum conct_type_description
{
	conct_type_description_value,
	conct_type_description_interface,
	conct_type_description_struct,
	conct_type_description_enum,
	conct_type_description_array
};

typedef uint16_t conct_type_crc;

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

extern conct_type_collection_handle		conct_type_collection_create();
extern void								conct_type_collection_destroy( conct_type_collection_handle handle );

extern bool								conct_type_collection_load( conct_type_collection_handle handle, const char* pPath );

extern conct_type_handle				conct_type_collection_add_value_type( conct_type_collection_handle handle, const char* pNamespace, const char* pName, const char* pCppName, conct_value_type valueType );

extern conct_type_handle				conct_type_collection_find_type( conct_type_collection_handle handle, const char* pFullname, const char* pReferenceNamespace );
extern conct_type_handle				conct_type_collection_find_type_by_crc( conct_type_collection_handle handle, conct_type_crc typeCrc );
extern conct_interface_type_handle		conct_type_collection_find_interface( conct_type_collection_handle handle, const char* pFullname, const char* pReferenceNamespace );
extern conct_interface_type_handle		conct_type_collection_find_interface_by_crc( conct_type_collection_handle handle, conct_type_crc typeCrc );
extern conct_struct_type_handle			conct_type_collection_find_struct( conct_type_collection_handle handle, const char* pFullname, const char* pReferenceNamespace );

extern conct_array_type_handle			conct_type_collection_make_array( conct_type_collection_handle handle, conct_type_handle typeHandle );

extern int								conct_type_collection_get_type_count( conct_type_collection_handle handle );
extern conct_type_handle				conct_type_collection_find_type_by_index( conct_type_collection_handle handle, int index );

extern int								conct_type_collection_get_interface_count( conct_type_collection_handle handle );
extern conct_interface_type_handle		conct_type_collection_find_interface_by_index( conct_type_collection_handle handle, int index );

extern int								conct_type_collection_get_struct_count( conct_type_collection_handle handle );
extern conct_struct_type_handle			conct_type_collection_find_struct_by_index( conct_type_collection_handle handle, int index );

// Type

extern conct_interface_type_handle		conct_type_cast_interface( conct_type_handle handle );
extern conct_struct_type_handle			conct_type_cast_struct( conct_type_handle handle );
extern conct_array_type_handle			conct_type_cast_array( conct_type_handle handle );

extern const char*						conct_type_get_namespace( conct_type_handle handle );
extern const char*						conct_type_get_name( conct_type_handle handle );
extern const char*						conct_type_get_cpp_name( conct_type_handle handle );
extern const char*						conct_type_get_full_name( conct_type_handle handle );
extern const char*						conct_type_get_header_filename( conct_type_handle handle );

extern conct_type_description			conct_type_get_description( conct_type_handle handle );
extern conct_value_type					conct_type_get_value_type( conct_type_handle handle );
extern conct_type_crc					conct_type_get_crc( conct_type_handle handle );

// InterfaceType

extern conct_type_handle				conct_interface_type_cast_type( conct_interface_type_handle handle );

extern conct_interface_type_handle		conct_interface_type_get_base_type( conct_interface_type_handle handle );

extern bool								conct_interface_type_get_internal( conct_interface_type_handle handle );
extern bool								conct_interface_type_get_singleton( conct_interface_type_handle handle );

extern int								conct_interface_type_get_property_count( conct_interface_type_handle handle );
extern const char*						conct_interface_type_get_property_name( conct_interface_type_handle handle, int index );
extern conct_type_handle				conct_interface_type_get_property_type( conct_interface_type_handle handle, int index );
extern bool								conct_interface_type_get_property_has_getter( conct_interface_type_handle handle, int index );
extern bool								conct_interface_type_get_property_has_setter( conct_interface_type_handle handle, int index );

extern int								conct_interface_type_get_function_count( conct_interface_type_handle handle );
extern const char*						conct_interface_type_get_function_name( conct_interface_type_handle handle, int index );
extern conct_type_handle				conct_interface_type_get_function_return_type( conct_interface_type_handle handle, int index );
extern int								conct_interface_type_get_function_parameter_count( conct_interface_type_handle handle, int index );

extern const char*						conct_interface_type_get_function_parameter_name( conct_interface_type_handle handle, int functionIndex, int parameterIndex );
extern conct_type_handle				conct_interface_type_get_function_parameter_type( conct_interface_type_handle handle, int functionIndex, int parameterIndex );

#ifdef __cplusplus
}
#endif
