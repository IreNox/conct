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

// Controller

extern conct_value_command_handle	conct_controller_get_property( conct_controller_handle handle, conct_device_address_handle addressHandle, conct_instance_id instanceId, const char* pName );
extern conct_command_handle			conct_controller_set_property( conct_controller_handle handle, conct_device_address_handle addressHandle, conct_instance_id instanceId, const char* pName, conct_value_handle valueHandle );
extern conct_value_command_handle	conct_controller_call_function( conct_controller_handle handle, conct_device_address_handle addressHandle, conct_instance_id instanceId, const char* pName, conct_value_handle* pValueHandles, int valueCount );

extern void							conct_controller_release_command( conct_controller_handle handle, conct_command_handle commandHandle );

// Command

extern bool							conct_command_is_busy( conct_command_handle handle );
extern bool							conct_command_is_finish( conct_command_handle handle );
extern bool							conct_command_is_ok( conct_command_handle handle );
extern bool							conct_command_is_failure( conct_command_handle handle );

// ValueCommand

extern conct_command_handle			conct_value_command_cast_command( conct_value_command_handle handle );

extern conct_value_handle			conct_value_command_get_value( conct_value_command_handle handle );

// DeviceAddress

//extern int							conct_device_address_create();

// Value

extern conct_value_handle			conct_value_create();
extern void							conct_value_destroy( conct_value_handle handle );

extern conct_value_type				conct_value_get_type( conct_value_handle handle );

extern bool							conct_value_get_boolean( conct_value_handle handle );
extern int							conct_value_get_integer( conct_value_handle handle );
extern unsigned						conct_value_get_unsigned( conct_value_handle handle );
extern const char*					conct_value_get_string( conct_value_handle handle );
extern conct_percent_value			conct_value_get_percent_value( conct_value_handle handle );
extern conct_guid					conct_value_get_guid( conct_value_handle handle );
extern conct_device_id				conct_value_get_device_id( conct_value_handle handle );
extern struct conct_instance		conct_value_get_instance( conct_value_handle handle );
extern conct_type_crc				conct_value_get_type_crc( conct_value_handle handle );

extern const void*					conct_value_get_struct_data( conct_value_handle handle );
extern int							conct_value_get_struct_size( conct_value_handle handle );
extern conct_type_crc				conct_value_get_struct_type( conct_value_handle handle );

extern const void*					conct_value_get_array_data( conct_value_handle handle );
extern int							conct_value_get_array_element_size( conct_value_handle handle );
extern int							conct_value_get_array_length( conct_value_handle handle );
extern conct_type_crc				conct_value_get_array_type( conct_value_handle handle );

extern void							conct_value_set_void( conct_value_handle handle );
extern void							conct_value_set_boolean( conct_value_handle handle, bool value );
extern void							conct_value_set_integer( conct_value_handle handle, int value );
extern void							conct_value_set_unsigned( conct_value_handle handle, unsigned value );
extern void							conct_value_set_string( conct_value_handle handle, const char* pValue );
extern void							conct_value_set_percent_value( conct_value_handle handle, conct_percent_value value );
extern void							conct_value_set_guid( conct_value_handle handle, conct_guid value );
extern void							conct_value_set_device_id( conct_value_handle handle, conct_device_id value );
extern void							conct_value_set_instance( conct_value_handle handle, struct conct_instance value );
extern void							conct_value_set_type_crc( conct_value_handle handle, conct_type_crc value );

#ifdef __cplusplus
}
#endif
