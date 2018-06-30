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

conct_value_handle toHandle( const ValueHigh* pClass )
{
	return ( conct_value_handle )pClass;
}

// Controller

conct_value_command_handle CONCT_CDECL conct_controller_get_property( conct_controller_handle handle, conct_device_address_handle addressHandle, conct_instance_id instanceId, const char* pName )
{
	RemoteInstance instance;
	runtime::copyDeviceAddress( instance.address, *fromHandle( addressHandle ) );
	instance.id = instanceId;

	return toHandle( fromHandle( handle )->getProperty( instance, pName ) );
}

conct_command_handle CONCT_CDECL conct_controller_set_property( conct_controller_handle handle, conct_device_address_handle addressHandle, conct_instance_id instanceId, const char* pName, conct_value_handle valueHandle )
{
	RemoteInstance instance;
	runtime::copyDeviceAddress( instance.address, *fromHandle( addressHandle ) );
	instance.id = instanceId;

	return toHandle( fromHandle( handle )->setProperty( instance, pName, *fromHandle( valueHandle ) ) );
}

conct_value_command_handle CONCT_CDECL conct_controller_call_function( conct_controller_handle handle, conct_device_address_handle addressHandle, conct_instance_id instanceId, const char* pName, conct_value_handle* pValueHandles, int valueCount )
{
	RemoteInstance instance;
	runtime::copyDeviceAddress( instance.address, *fromHandle( addressHandle ) );
	instance.id = instanceId;

	ArrayView< ValueHigh > parameters;
	parameters.set( ( ValueHigh* )pValueHandles, uintreg( valueCount ) );

	return toHandle( fromHandle( handle )->callFunction( instance, pName, parameters ) );
}

void CONCT_CDECL conct_controller_release_command( conct_controller_handle handle, conct_command_handle commandHandle )
{
	fromHandle( handle )->releaseCommand( fromHandle( commandHandle ) );
}

// Command

bool CONCT_CDECL conct_command_is_busy( conct_command_handle handle )
{
	return fromHandle( handle )->isBusy();
}

bool CONCT_CDECL conct_command_is_finish( conct_command_handle handle )
{
	return fromHandle( handle )->isFinish();
}

bool CONCT_CDECL conct_command_is_ok( conct_command_handle handle )
{
	return fromHandle( handle )->isOk();
}

bool CONCT_CDECL conct_command_is_failure( conct_command_handle handle )
{
	return fromHandle( handle )->isFailure();
}

// ValueCommand

extern conct_command_handle conct_value_command_cast_command( conct_value_command_handle handle )
{
	return ( conct_command_handle )handle;
}

conct_value_handle CONCT_CDECL conct_value_command_get_value( conct_value_command_handle handle )
{
	ValueHigh* pValue = new ValueHigh( fromHandle(handle)->getValue() );
	return toHandle( pValue );
}

// DeviceAddress


// Value

conct_value_handle CONCT_CDECL conct_value_create()
{
	return toHandle( new ValueHigh() );
}

void CONCT_CDECL conct_value_destroy( conct_value_handle handle )
{
	delete fromHandle( handle );
}

conct_value_type CONCT_CDECL conct_value_get_type( conct_value_handle handle )
{
	return ( conct_value_type )fromHandle( handle )->getType();
}

bool CONCT_CDECL conct_value_get_boolean( conct_value_handle handle )
{
	return fromHandle( handle )->getBoolean();
}

int CONCT_CDECL conct_value_get_integer( conct_value_handle handle )
{
	return fromHandle( handle )->getInteger();
}

unsigned CONCT_CDECL conct_value_get_unsigned( conct_value_handle handle )
{
	return fromHandle( handle )->getUnsigned();
}

const char* CONCT_CDECL conct_value_get_string( conct_value_handle handle )
{
	return fromHandle( handle )->getString();
}

conct_percent_value CONCT_CDECL conct_value_get_percent_value( conct_value_handle handle )
{
	return fromHandle( handle )->getPercentValue();
}

conct_guid CONCT_CDECL conct_value_get_guid( conct_value_handle handle )
{
	return fromHandle( handle )->getGuid();
}

conct_device_id CONCT_CDECL conct_value_get_device_id( conct_value_handle handle )
{
	return fromHandle( handle )->getDeviceId();
}

struct conct_instance CONCT_CDECL conct_value_get_instance( conct_value_handle handle )
{
	const Instance instance = fromHandle( handle )->getInstance();
	return *( conct_instance* )&instance;
}

conct_type_crc CONCT_CDECL conct_value_get_type_crc( conct_value_handle handle )
{
	return fromHandle( handle )->getTypeCrc();
}

const void* CONCT_CDECL conct_value_get_struct_data( conct_value_handle handle )
{
	return fromHandle( handle )->getStructData();
}

int CONCT_CDECL conct_value_get_struct_size( conct_value_handle handle )
{
	return fromHandle( handle )->getStructSize();
}

conct_type_crc CONCT_CDECL conct_value_get_struct_type( conct_value_handle handle )
{
	return fromHandle( handle )->getStructType();
}

const void* CONCT_CDECL conct_value_get_array_data( conct_value_handle handle )
{
	return fromHandle( handle )->getArrayData();
}

int CONCT_CDECL conct_value_get_array_element_size( conct_value_handle handle )
{
	return fromHandle( handle )->getArrayElementSize();
}

int CONCT_CDECL conct_value_get_array_length( conct_value_handle handle )
{
	return fromHandle( handle )->getArrayLength();
}

conct_type_crc CONCT_CDECL conct_value_get_array_type( conct_value_handle handle )
{
	return fromHandle( handle )->getArrayType();
}

void CONCT_CDECL conct_value_set_void( conct_value_handle handle )
{
	fromHandle( handle )->setVoid();
}

void CONCT_CDECL conct_value_set_boolean( conct_value_handle handle, bool value )
{
	fromHandle( handle )->setBoolean( value );
}

void CONCT_CDECL conct_value_set_integer( conct_value_handle handle, int value )
{
	fromHandle( handle )->setInteger( value );
}

void CONCT_CDECL conct_value_set_unsigned( conct_value_handle handle, unsigned value )
{
	fromHandle( handle )->setUnsigned( value );
}

void CONCT_CDECL conct_value_set_percent_value( conct_value_handle handle, conct_percent_value value )
{
	fromHandle( handle )->setPercentValue( value );
}

void CONCT_CDECL conct_value_set_guid( conct_value_handle handle, conct_guid value )
{
	fromHandle( handle )->setGuid( value );
}

void CONCT_CDECL conct_value_set_device_id( conct_value_handle handle, conct_device_id value )
{
	fromHandle( handle )->setDeviceId( value );
}

void CONCT_CDECL conct_value_set_instance( conct_value_handle handle, struct conct_instance value )
{
	fromHandle( handle )->setInstance( *(Instance*)&value );
}

void CONCT_CDECL conct_value_set_type_crc( conct_value_handle handle, conct_type_crc value )
{
	fromHandle( handle )->setTypeCrc( value );
}
