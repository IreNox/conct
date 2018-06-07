#include "core_device_proxy.h"

#include "conct_string.h"

#include "core_device.h"

namespace conct
{
	DeviceProxy::DeviceProxy()
		: Proxy( Device::s_typeCrc )
	{
	}

	bool DeviceProxy::getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, const char* pName ) const
	{
		const Device* pTypedInstance = static_cast< const Device* >( pInstance );

		if( isStringEquals( pName, "Name" ) )
		{
			targetValueBuilder.setString( pTypedInstance->getName() );
			return true;
		}
		if( isStringEquals( pName, "Guid" ) )
		{
			targetValueBuilder.setGuid( pTypedInstance->getGuid() );
			return true;
		}
		if( isStringEquals( pName, "Instances" ) )
		{
			targetValueBuilder.setArray( pTypedInstance->getInstances() );
			return true;
		}

		return false;
	}

	bool DeviceProxy::setProperty( void* pInstance, const char* pName, const Value& value )
	{
		Device* pTypedInstance = static_cast< Device* >( pInstance );


		return false;
	}

	bool DeviceProxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, const char* pName, const ArrayView< Value >& parameters )
	{
		const Device* pTypedInstance = static_cast< Device* >( pInstance );

		if( isStringEquals( pName, "FindInstances" ) )
		{
			targetValueBuilder.setArray( pTypedInstance->FindInstances( parameters[ 0u ].getTypeCrc() ) );
			return true;
		}
		if( isStringEquals( pName, "FinstFirstInstance" ) )
		{
			targetValueBuilder.setInstanceId( pTypedInstance->FinstFirstInstance( parameters[ 0u ].getTypeCrc() ) );
			return true;
		}

		return false;
	}
}
