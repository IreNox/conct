#pragma once

#include "conct_core_c.h"

#ifdef __cplusplus
extern "C" {
#endif

// Types

struct conct_device;
typedef struct conct_device* conct_device_handle;

struct conct_port;
typedef struct conct_port* conct_port_handle;

struct conct_controller;
typedef struct conct_controller* conct_controller_handle;

struct conct_command;
typedef struct conct_command* conct_command_handle;

struct conct_value_command;
typedef struct conct_value_command* conct_value_command_handle;

struct conct_device_address;
typedef struct conct_device_address* conct_device_address_handle;

struct conct_value;
typedef struct conct_value* conct_value_handle;

typedef void( *conct_controller_command_callback )( conct_command_handle commandHandle );

// Controller

CONCT_DLL conct_value_command_handle	CONCT_CDECL conct_controller_get_property( conct_controller_handle handle, conct_device_address_handle addressHandle, conct_instance_id instanceId, const char* pName );
CONCT_DLL conct_command_handle			CONCT_CDECL conct_controller_set_property( conct_controller_handle handle, conct_device_address_handle addressHandle, conct_instance_id instanceId, const char* pName, conct_value_handle valueHandle );
CONCT_DLL conct_value_command_handle	CONCT_CDECL conct_controller_call_function( conct_controller_handle handle, conct_device_address_handle addressHandle, conct_instance_id instanceId, const char* pName, conct_value_handle* pValueHandles, int valueCount );

CONCT_DLL void							CONCT_CDECL conct_controller_release_command( conct_controller_handle handle, conct_command_handle commandHandle );

CONCT_DLL void							CONCT_CDECL conct_controller_register_command_callback( conct_controller_handle handle, conct_controller_command_callback callback );
CONCT_DLL void							CONCT_CDECL conct_controller_unregister_command_callback( conct_controller_handle handle, conct_controller_command_callback callback );

// Command

CONCT_DLL bool							CONCT_CDECL conct_command_is_busy( conct_command_handle handle );
CONCT_DLL bool							CONCT_CDECL conct_command_is_finish( conct_command_handle handle );
CONCT_DLL bool							CONCT_CDECL conct_command_is_ok( conct_command_handle handle );
CONCT_DLL bool							CONCT_CDECL conct_command_is_failure( conct_command_handle handle );

// ValueCommand

CONCT_DLL conct_command_handle			CONCT_CDECL conct_value_command_cast_command( conct_value_command_handle handle );

CONCT_DLL conct_value_handle			CONCT_CDECL conct_value_command_get_value( conct_value_command_handle handle );

// DeviceAddress

CONCT_DLL conct_device_address_handle	CONCT_CDECL conct_device_address_create();
CONCT_DLL conct_device_address_handle	CONCT_CDECL conct_device_address_duplicate( conct_device_address_handle handle );
CONCT_DLL void							CONCT_CDECL conct_device_address_destroy( conct_device_address_handle handle );

CONCT_DLL int							CONCT_CDECL conct_device_address_get_device_count( conct_device_address_handle handle );
CONCT_DLL conct_device_id				CONCT_CDECL conct_device_address_get_device_id( conct_device_address_handle handle, int index );

CONCT_DLL void							CONCT_CDECL conct_device_address_push_device( conct_device_address_handle handle, conct_device_id deviceId );
CONCT_DLL conct_device_id				CONCT_CDECL conct_device_address_pop_device( conct_device_address_handle handle );

CONCT_DLL bool							CONCT_CDECL conct_device_address_is_empty( conct_device_address_handle handle );
CONCT_DLL bool							CONCT_CDECL conct_device_address_is_equals( conct_device_address_handle handle1, conct_device_address_handle handle2 );

// Value

CONCT_DLL conct_value_handle			CONCT_CDECL conct_value_create();
CONCT_DLL void							CONCT_CDECL conct_value_destroy( conct_value_handle handle );

CONCT_DLL conct_value_type				CONCT_CDECL conct_value_get_type( conct_value_handle handle );

CONCT_DLL bool							CONCT_CDECL conct_value_get_boolean( conct_value_handle handle );
CONCT_DLL int							CONCT_CDECL conct_value_get_integer( conct_value_handle handle );
CONCT_DLL unsigned						CONCT_CDECL conct_value_get_unsigned( conct_value_handle handle );
CONCT_DLL const char*					CONCT_CDECL conct_value_get_string( conct_value_handle handle );
CONCT_DLL conct_percent_value			CONCT_CDECL conct_value_get_percent_value( conct_value_handle handle );
CONCT_DLL conct_guid					CONCT_CDECL conct_value_get_guid( conct_value_handle handle );
CONCT_DLL conct_device_id				CONCT_CDECL conct_value_get_device_id( conct_value_handle handle );
CONCT_DLL struct conct_instance			CONCT_CDECL conct_value_get_instance( conct_value_handle handle );
CONCT_DLL conct_type_crc				CONCT_CDECL conct_value_get_type_crc( conct_value_handle handle );

CONCT_DLL const void*					CONCT_CDECL conct_value_get_struct_data( conct_value_handle handle );
CONCT_DLL int							CONCT_CDECL conct_value_get_struct_size( conct_value_handle handle );
CONCT_DLL conct_type_crc				CONCT_CDECL conct_value_get_struct_type( conct_value_handle handle );

CONCT_DLL const void*					CONCT_CDECL conct_value_get_array_data( conct_value_handle handle );
CONCT_DLL int							CONCT_CDECL conct_value_get_array_element_size( conct_value_handle handle );
CONCT_DLL int							CONCT_CDECL conct_value_get_array_length( conct_value_handle handle );
CONCT_DLL conct_type_crc				CONCT_CDECL conct_value_get_array_type( conct_value_handle handle );

CONCT_DLL void							CONCT_CDECL conct_value_set_void( conct_value_handle handle );
CONCT_DLL void							CONCT_CDECL conct_value_set_boolean( conct_value_handle handle, bool value );
CONCT_DLL void							CONCT_CDECL conct_value_set_integer( conct_value_handle handle, int value );
CONCT_DLL void							CONCT_CDECL conct_value_set_unsigned( conct_value_handle handle, unsigned value );
CONCT_DLL void							CONCT_CDECL conct_value_set_string( conct_value_handle handle, const char* pValue );
CONCT_DLL void							CONCT_CDECL conct_value_set_percent_value( conct_value_handle handle, conct_percent_value value );
CONCT_DLL void							CONCT_CDECL conct_value_set_guid( conct_value_handle handle, conct_guid value );
CONCT_DLL void							CONCT_CDECL conct_value_set_device_id( conct_value_handle handle, conct_device_id value );
CONCT_DLL void							CONCT_CDECL conct_value_set_instance( conct_value_handle handle, struct conct_instance value );
CONCT_DLL void							CONCT_CDECL conct_value_set_type_crc( conct_value_handle handle, conct_type_crc value );
CONCT_DLL void							CONCT_CDECL conct_value_set_struct( conct_value_handle handle, const void* pData, int size, conct_type_crc type );
CONCT_DLL void							CONCT_CDECL conct_value_set_array( conct_value_handle handle, const void* pData, int elementSize, int length, conct_type_crc type );

#ifdef __cplusplus
}
#endif
