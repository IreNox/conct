#include "core_device_proxy.h"

#include "conct_string.h"

#include "conct_device.h"
#include "conct_array_view.h"
#include "conct_core.h"
#include "core_instance.h"

namespace conct
{
	DeviceProxy::DeviceProxy()
		: Proxy( 32636 )
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

	bool DeviceProxy::setProperty( void* pInstance, const char* pName, const Value& value ) const
	{
		Device* pTypedInstance = static_cast< Device* >( pInstance );


		return false;
	}

	bool DeviceProxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, const char* pName, const ArrayView< Value >& parameters ) const
	{
		Device* pTypedInstance = static_cast< Device* >( pInstance );

		if( isStringEquals( pName, "findInstances" ) )
		{
			targetValueBuilder.setArray( pTypedInstance->findInstances( parameters[ 0u ].getTypeCrc() ) );
			return true;
		}
		if( isStringEquals( pName, "findFirstInstance" ) )
		{
			targetValueBuilder.setInstanceId( pTypedInstance->findFirstInstance( parameters[ 0u ].getTypeCrc() ) );
			return true;
		}

		return false;
	}
}
