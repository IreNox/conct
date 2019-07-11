#include "conct_runtime_high_c.h"

#include "conct_controller.h"

using namespace conct;

Controller* fromHandle( conct_controller_handle handle )
{
	return ( Controller* )handle;
}

Command* fromHandle( conct_command_handle handle )
{
	return ( Command* )handle;
}

ValueCommand* fromHandle( conct_value_command_handle handle )
{
	return ( ValueCommand* )handle;
}

DeviceAddress* fromHandle( conct_device_address_handle handle )
{
	return ( DeviceAddress* )handle;
}

ValueHigh* fromHandle( conct_value_handle handle )
{
	return ( ValueHigh* )handle;
}

conct_command_handle toHandle( const Command* pClass )
{
	return ( conct_command_handle )pClass;
}

conct_value_command_handle toHandle( const ValueCommand* pClass )
{
	return ( conct_value_command_handle )pClass;
}

conct_device_address_handle toHandle( const DeviceAddress* pClass )
{
	return ( conct_device_address_handle )pClass;
}

conct_value_handle toHandle( const ValueHigh* pClass )
{
	return ( conct_value_handle )pClass;
}

// Controller

CONCT_DLL conct_value_command_handle CONCT_CDECL conct_controller_get_property( conct_controller_handle handle, conct_device_address_handle addressHandle, conct_instance_id instanceId, const char* pName )
{
	RemoteInstance instance;
	runtime::copyDeviceAddress( instance.address, *fromHandle( addressHandle ) );
	instance.id = instanceId;

	return toHandle( fromHandle( handle )->getProperty( instance, pName ) );
}

CONCT_DLL conct_command_handle CONCT_CDECL conct_controller_set_property( conct_controller_handle handle, conct_device_address_handle addressHandle, conct_instance_id instanceId, const char* pName, conct_value_handle valueHandle )
{
	RemoteInstance instance;
	runtime::copyDeviceAddress( instance.address, *fromHandle( addressHandle ) );
	instance.id = instanceId;

	return toHandle( fromHandle( handle )->setProperty( instance, pName, *fromHandle( valueHandle ) ) );
}

CONCT_DLL conct_value_command_handle CONCT_CDECL conct_controller_call_function( conct_controller_handle handle, conct_device_address_handle addressHandle, conct_instance_id instanceId, const char* pName, conct_value_handle* pValueHandles, int valueCount )
{
	RemoteInstance instance;
	runtime::copyDeviceAddress( instance.address, *fromHandle( addressHandle ) );
	instance.id = instanceId;

	ArrayView< ValueHigh > parameters;
	parameters.set( ( ValueHigh* )pValueHandles, uintreg( valueCount ) );

	return toHandle( fromHandle( handle )->callFunction( instance, pName, parameters ) );
}

CONCT_DLL void CONCT_CDECL conct_controller_release_command( conct_controller_handle handle, conct_command_handle commandHandle )
{
	fromHandle( handle )->releaseCommand( fromHandle( commandHandle ) );
}

CONCT_DLL void CONCT_CDECL conct_controller_register_command_callback( conct_controller_handle handle, conct_controller_command_callback callback )
{
	fromHandle( handle )->registerCommandCallback( (ControllerCommandCallback)callback );
}

CONCT_DLL void CONCT_CDECL conct_controller_unregister_command_callback( conct_controller_handle handle, conct_controller_command_callback callback )
{
	fromHandle( handle )->unregisterCommandCallback( (ControllerCommandCallback)callback );
}

// Command

CONCT_DLL bool CONCT_CDECL conct_command_is_busy( conct_command_handle handle )
{
	return fromHandle( handle )->isBusy();
}

CONCT_DLL bool CONCT_CDECL conct_command_is_finish( conct_command_handle handle )
{
	return fromHandle( handle )->isFinish();
}

CONCT_DLL bool CONCT_CDECL conct_command_is_ok( conct_command_handle handle )
{
	return fromHandle( handle )->isOk();
}

CONCT_DLL bool CONCT_CDECL conct_command_is_failure( conct_command_handle handle )
{
	return fromHandle( handle )->isFailure();
}

// ValueCommand

CONCT_DLL conct_command_handle CONCT_CDECL conct_value_command_cast_command( conct_value_command_handle handle )
{
	return ( conct_command_handle )handle;
}

CONCT_DLL conct_value_handle CONCT_CDECL conct_value_command_get_value( conct_value_command_handle handle )
{
	ValueHigh* pValue = new ValueHigh( fromHandle(handle)->getValue() );
	return toHandle( pValue );
}

// DeviceAddress

CONCT_DLL conct_device_address_handle CONCT_CDECL conct_device_address_create()
{
	DeviceAddress* pAddress = new DeviceAddress();
	pAddress->address[ 0u ] = InvalidDeviceId;
	return toHandle( pAddress );
}

CONCT_DLL conct_device_address_handle CONCT_CDECL conct_device_address_duplicate( conct_device_address_handle handle )
{
	DeviceAddress* pAddress = new DeviceAddress();
	*pAddress = *fromHandle( handle );
	return toHandle( pAddress );
}

CONCT_DLL void CONCT_CDECL conct_device_address_destroy( conct_device_address_handle handle )
{
	delete fromHandle( handle );
}

CONCT_DLL int CONCT_CDECL conct_device_address_get_device_count( conct_device_address_handle handle )
{
	DeviceAddress* pAdress = fromHandle( handle );
	for( int i = 0u; i < DeviceAddress::Size; ++i )
	{
		if( pAdress->address[ i ] == InvalidDeviceId )
		{
			return i;
		}
	}

	return 0u;
}

CONCT_DLL conct_device_id CONCT_CDECL conct_device_address_get_device_id( conct_device_address_handle handle, int index )
{
	return fromHandle( handle )->address[ index ];
}

CONCT_DLL void CONCT_CDECL conct_device_address_push_device( conct_device_address_handle handle, conct_device_id deviceId )
{
	DeviceAddress* pAdress = fromHandle( handle );
	for( uintreg i = 0u; i < DeviceAddress::Size - 1u; ++i )
	{
		if( pAdress->address[ i ] == InvalidDeviceId )
		{
			pAdress->address[ i ] = deviceId;
			pAdress->address[ i + 1u ] = InvalidDeviceId;
			return;
		}
	}
}

CONCT_DLL conct_device_id CONCT_CDECL conct_device_address_pop_device( conct_device_address_handle handle )
{
	DeviceAddress* pAdress = fromHandle( handle );
	for( uintreg i = 1u; i < DeviceAddress::Size; ++i )
	{
		if( pAdress->address[ i ] == InvalidDeviceId )
		{
			const DeviceId deviceId = pAdress->address[ i - 1u ];
			pAdress->address[ i - 1u ] = InvalidDeviceId;
			return deviceId;
		}
	}

	return InvalidDeviceId;
}

CONCT_DLL bool CONCT_CDECL conct_device_address_is_empty( conct_device_address_handle handle )
{
	return runtime::isDeviceAddressEmpty( *fromHandle( handle ) );
}

CONCT_DLL bool CONCT_CDECL conct_device_address_is_equals( conct_device_address_handle handle1, conct_device_address_handle handle2 )
{
	return runtime::isDeviceAddressEquals( *fromHandle( handle1 ), *fromHandle( handle2 ) );
}

// Value

CONCT_DLL conct_value_handle CONCT_CDECL conct_value_create()
{
	return toHandle( new ValueHigh() );
}

CONCT_DLL void CONCT_CDECL conct_value_destroy( conct_value_handle handle )
{
	delete fromHandle( handle );
}

CONCT_DLL conct_value_type CONCT_CDECL conct_value_get_type( conct_value_handle handle )
{
	return ( conct_value_type )fromHandle( handle )->getType();
}

CONCT_DLL bool CONCT_CDECL conct_value_get_boolean( conct_value_handle handle )
{
	return fromHandle( handle )->getBoolean();
}

CONCT_DLL int CONCT_CDECL conct_value_get_integer( conct_value_handle handle )
{
	return fromHandle( handle )->getInteger();
}

CONCT_DLL unsigned CONCT_CDECL conct_value_get_unsigned( conct_value_handle handle )
{
	return fromHandle( handle )->getUnsigned();
}

CONCT_DLL const char* CONCT_CDECL conct_value_get_string( conct_value_handle handle )
{
	return fromHandle( handle )->getString();
}

CONCT_DLL conct_percent_value CONCT_CDECL conct_value_get_percent_value( conct_value_handle handle )
{
	return fromHandle( handle )->getPercentValue();
}

CONCT_DLL conct_device_id CONCT_CDECL conct_value_get_device_id( conct_value_handle handle )
{
	return fromHandle( handle )->getDeviceId();
}

CONCT_DLL conct_type_crc CONCT_CDECL conct_value_get_type_crc( conct_value_handle handle )
{
	return fromHandle( handle )->getTypeCrc();
}

CONCT_DLL const void* CONCT_CDECL conct_value_get_struct_data( conct_value_handle handle )
{
	return fromHandle( handle )->getStructData();
}

CONCT_DLL int CONCT_CDECL conct_value_get_struct_size( conct_value_handle handle )
{
	return int( fromHandle( handle )->getStructSize() );
}

CONCT_DLL conct_type_crc CONCT_CDECL conct_value_get_struct_type( conct_value_handle handle )
{
	return fromHandle( handle )->getStructType();
}

CONCT_DLL const void* CONCT_CDECL conct_value_get_array_data( conct_value_handle handle )
{
	return fromHandle( handle )->getArrayData();
}

CONCT_DLL int CONCT_CDECL conct_value_get_array_element_size( conct_value_handle handle )
{
	return int( fromHandle( handle )->getArrayElementSize() );
}

CONCT_DLL int CONCT_CDECL conct_value_get_array_length( conct_value_handle handle )
{
	return int( fromHandle( handle )->getArrayLength() );
}

CONCT_DLL conct_type_crc CONCT_CDECL conct_value_get_array_type( conct_value_handle handle )
{
	return fromHandle( handle )->getArrayType();
}

CONCT_DLL void CONCT_CDECL conct_value_set_void( conct_value_handle handle )
{
	fromHandle( handle )->setVoid();
}

CONCT_DLL void CONCT_CDECL conct_value_set_boolean( conct_value_handle handle, bool value )
{
	fromHandle( handle )->setBoolean( value );
}

CONCT_DLL void CONCT_CDECL conct_value_set_integer( conct_value_handle handle, int value )
{
	fromHandle( handle )->setInteger( value );
}

CONCT_DLL void CONCT_CDECL conct_value_set_unsigned( conct_value_handle handle, unsigned value )
{
	fromHandle( handle )->setUnsigned( value );
}

CONCT_DLL void CONCT_CDECL conct_value_set_string( conct_value_handle handle, const char* pValue )
{
	fromHandle( handle )->setString( pValue );
}

CONCT_DLL void CONCT_CDECL conct_value_set_percent_value( conct_value_handle handle, conct_percent_value value )
{
	fromHandle( handle )->setPercentValue( value );
}

CONCT_DLL void CONCT_CDECL conct_value_set_device_id( conct_value_handle handle, conct_device_id value )
{
	fromHandle( handle )->setDeviceId( value );
}

CONCT_DLL void CONCT_CDECL conct_value_set_type_crc( conct_value_handle handle, conct_type_crc value )
{
	fromHandle( handle )->setTypeCrc( value );
}

CONCT_DLL void CONCT_CDECL conct_value_set_struct( conct_value_handle handle, const void* pData, int size, conct_type_crc type )
{
	fromHandle( handle )->setStruct( pData, size, type );
}

CONCT_DLL void CONCT_CDECL conct_value_set_array( conct_value_handle handle, const void* pData, int elementSize, int length, conct_type_crc type )
{
	fromHandle( handle )->setArray( pData, elementSize, length, type );
}
