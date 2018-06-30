#include "device_c.h"

#include "device_android_controller.h"

using namespace conct;

DeviceAndroidController* fromHandle( conct_device_handle handle )
{
	return ( DeviceAndroidController* )handle;
}

conct_device_handle toHandle( const DeviceAndroidController* pClass )
{
	return ( conct_device_handle )pClass;
}

conct_controller_handle toHandle( const Controller* pClass )
{
	return ( conct_controller_handle )pClass;
}

conct_device_handle CONCT_CDECL conct_device_create()
{
	DeviceAndroidController* pDevice = new DeviceAndroidController();
	pDevice->setupDevice();

	return toHandle( pDevice );
}

void CONCT_CDECL conct_device_destroy( conct_device_handle handle )
{
	delete fromHandle( handle );
}

void CONCT_CDECL conct_device_loop( conct_device_handle handle )
{
	fromHandle( handle )->loopDevice();
}

conct_port_handle CONCT_CDECL conct_device_add_port( conct_device_handle handle, const char* pHostname, uint16_t hostPort )
{
	return nullptr;
}

void CONCT_CDECL conct_device_remove_port( conct_device_handle handle, conct_port_handle portHandle )
{

}

conct_controller_handle CONCT_CDECL conct_device_get_controller( conct_device_handle handle )
{
	return toHandle( &fromHandle( handle )->getController() );
}
