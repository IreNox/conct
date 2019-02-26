#include "core_router_proxy.h"

#include "conct_router.h"
#include "conct_array_view.h"
#include "conct_core.h"

namespace conct
{
	RouterProxy::RouterProxy()
		: Proxy( 25653 )
	{
	}

	bool RouterProxy::getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, uint16 nameCrc ) const
	{
		const Router* pTypedInstance = static_cast< const Router* >( pInstance );

		if( nameCrc == 0x1e4b )
		{
			targetValueBuilder.setArray( pTypedInstance->getConnectedDevices() );
			return true;
		}

		return false;
	}

	bool RouterProxy::setProperty( void* pInstance, uint16 nameCrc, const Value& value ) const
	{
		return false;
	}

	bool RouterProxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, uint16 nameCrc, const ArrayView< Value >& parameters ) const
	{
		return false;
	}
}
