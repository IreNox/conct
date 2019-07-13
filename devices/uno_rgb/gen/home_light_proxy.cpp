#include "home_light_proxy.h"

#include "home_light.h"
#include "conct_core.h"

namespace conct
{
	LightProxy::LightProxy()
		: Proxy( Light::s_typeCrc )
	{
	}

	bool LightProxy::getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, uint16 nameCrc ) const
	{
		const Light* pTypedInstance = static_cast< const Light* >( pInstance );

		if( nameCrc == 0xdcb5 )
		{
			targetValueBuilder.setBoolean( pTypedInstance->getOn() );
			return true;
		}

		return false;
	}

	bool LightProxy::setProperty( void* pInstance, uint16 nameCrc, const Value& value ) const
	{
		Light* pTypedInstance = static_cast< Light* >( pInstance );

		if( nameCrc == 0xdcb5 )
		{
			pTypedInstance->setOn( value.getBoolean() );
			return true;
		}

		return false;
	}

	bool LightProxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, uint16 nameCrc, const ArrayView< Value >& parameters ) const
	{
		return false;
	}
}
