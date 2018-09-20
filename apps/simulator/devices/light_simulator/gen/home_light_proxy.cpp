#include "home_light_proxy.h"

#include "conct_string.h"

#include "home_light.h"
#include "conct_core.h"

namespace conct
{
	LightProxy::LightProxy()
		: Proxy( Light::s_typeCrc )
	{
	}

	bool LightProxy::getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, const char* pName ) const
	{
		const Light* pTypedInstance = static_cast< const Light* >( pInstance );

		if( isStringEquals( pName, "On" ) )
		{
			targetValueBuilder.setBoolean( pTypedInstance->getOn() );
			return true;
		}

		return false;
	}

	bool LightProxy::setProperty( void* pInstance, const char* pName, const Value& value ) const
	{
		Light* pTypedInstance = static_cast< Light* >( pInstance );

		if( isStringEquals( pName, "On" ) )
		{
			pTypedInstance->setOn( value.getBoolean() );
			return true;
		}

		return false;
	}

	bool LightProxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, const char* pName, const ArrayView< Value >& parameters ) const
	{
		return false;
	}
}
