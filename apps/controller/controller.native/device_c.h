#pragma once

#include "conct_runtime_high_c.h"

#ifdef __cplusplus
extern "C" {
#endif

// Device

CONCT_DLL conct_device_handle			CONCT_CDECL conct_device_create();
CONCT_DLL void							CONCT_CDECL conct_device_destroy( conct_device_handle handle );

CONCT_DLL void							CONCT_CDECL conct_device_loop( conct_device_handle handle );

CONCT_DLL conct_port_handle				CONCT_CDECL conct_device_add_port( conct_device_handle handle, const char* pHostname, uint16_t hostPort );
CONCT_DLL void							CONCT_CDECL conct_device_remove_port( conct_device_handle handle, conct_port_handle portHandle );

CONCT_DLL conct_controller_handle		CONCT_CDECL conct_device_get_controller( conct_device_handle handle );

CONCT_DLL int							CONCT_CDECL conct_device_get_connected_device_count( conct_device_handle handle );
CONCT_DLL conct_device_address_handle	CONCT_CDECL conct_device_get_connected_device( conct_device_handle handle, int index );

#ifdef __cplusplus
}
#endif
