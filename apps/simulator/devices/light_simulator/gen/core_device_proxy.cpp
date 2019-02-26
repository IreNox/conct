#include "core_device_proxy.h"

#include "conct_device.h"
#include "conct_core.h"
#include "conct_array_view.h"

namespace conct
{
	DeviceProxy::DeviceProxy()
		: Proxy( 32636 )
	{
	}

	bool DeviceProxy::getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, uint16 nameCrc ) const
	{
		const Device* pTypedInstance = static_cast< const Device* >( pInstance );

		if( nameCrc == 0x4daa )
		{
			targetValueBuilder.setString( pTypedInstance->getName() );
			return true;
		}

		if( nameCrc == 0xd52a )
		{
			targetValueBuilder.setGuid( pTypedInstance->getGuid() );
			return true;
		}

		if( nameCrc == 0xcfaf )
		{
			targetValueBuilder.setArray( pTypedInstance->getInstances() );
			return true;
		}

		return false;
	}

	bool DeviceProxy::setProperty( void* pInstance, uint16 nameCrc, const Value& value ) const
	{
		return false;
	}

	bool DeviceProxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, uint16 nameCrc, const ArrayView< Value >& parameters ) const
	{
		Device* pTypedInstance = static_cast< Device* >( pInstance );

		if( nameCrc == 0xb733 )
		{
			targetValueBuilder.setArray( pTypedInstance->findInstances( parameters[ 0u ].getTypeCrc() ) );
			return true;
		}

		if( nameCrc == 0xe2a7 )
		{
			targetValueBuilder.setInstance( pTypedInstance->findFirstInstance( parameters[ 0u ].getTypeCrc() ) );
			return true;
		}

		return false;
	}
}
