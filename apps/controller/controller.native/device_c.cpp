#include "device_c.h"

#include "device_controller.h"

using namespace conct;

DeviceController* fromHandle( conct_device_handle handle )
{
	return (DeviceController* )handle;
}

PortTcpClient* fromHandle( conct_port_handle handle )
{
	return ( PortTcpClient* )handle;
}

conct_device_handle toHandle( const DeviceController* pClass )
{
	return ( conct_device_handle )pClass;
}

conct_port_handle toHandle( const PortTcpClient* pClass )
{
	return ( conct_port_handle )pClass;
}

conct_controller_handle toHandle( const Controller* pClass )
{
	return ( conct_controller_handle )pClass;
}

conct_device_handle CONCT_CDECL conct_device_create()
{
	DeviceController* pDevice = new DeviceController();
	pDevice->setupDevice();

	return toHandle( pDevice );
}

CONCT_DLL void CONCT_CDECL conct_device_destroy( conct_device_handle handle )
{
	delete fromHandle( handle );
}

CONCT_DLL void CONCT_CDECL conct_device_loop( conct_device_handle handle )
{
	fromHandle( handle )->loopDevice();
}

CONCT_DLL conct_port_handle CONCT_CDECL conct_device_add_port( conct_device_handle handle, const char* pHostname, uint16_t hostPort )
{
	return toHandle( fromHandle( handle )->addPort( pHostname, hostPort ) );
}

CONCT_DLL void CONCT_CDECL conct_device_remove_port( conct_device_handle handle, conct_port_handle portHandle )
{
	fromHandle( handle )->removePort( fromHandle( portHandle ) );
}

CONCT_DLL conct_controller_handle CONCT_CDECL conct_device_get_controller( conct_device_handle handle )
{
	return toHandle( &fromHandle( handle )->getController() );
}

CONCT_DLL int CONCT_CDECL conct_device_get_connected_device_count( conct_device_handle handle )
{
	return int( fromHandle( handle )->getRuntime().getDeviceCount() );
}

CONCT_DLL conct_device_address_handle CONCT_CDECL conct_device_get_connected_device( conct_device_handle handle, int index )
{
	Vector< DeviceConnection > connections;
	fromHandle( handle )->getRuntime().getDevices( connections );

	const DeviceId deviceId = connections[ index ].id;
	conct_device_address_handle addressHandle = conct_device_address_create();
	conct_device_address_push_device( addressHandle, deviceId );
	return addressHandle;
}

CONCT_DLL bool CONCT_CDECL conct_device_is_this_device( conct_device_handle handle, conct_device_address_handle addressHandle )
{
	return fromHandle( handle )->getRuntime().isThisDevice( *(DeviceAddress*)addressHandle );
}