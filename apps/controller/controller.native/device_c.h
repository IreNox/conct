#pragma once

#include "conct_runtime_high_c.h"

#ifdef __cplusplus
extern "C" {
#endif

// Device

extern conct_device_handle			conct_device_create();
extern void							conct_device_destroy( conct_device_handle handle );

extern void							conct_device_loop( conct_device_handle handle );

extern conct_port_handle			conct_device_add_port( conct_device_handle handle, const char* pHostname, uint16_t hostPort );
extern void							conct_device_remove_port( conct_device_handle handle, conct_port_handle portHandle );

extern conct_controller_handle		conct_device_get_controller( conct_device_handle handle );

#ifdef __cplusplus
}
#endif
