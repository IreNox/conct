#include "home_light_proxy.h"

#include "conct_string.h"

#include "home_light.h"

namespace conct
{
	LightProxy::LightProxy()
		: Proxy( Light::s_typeCrc )
	{
	}

	bool LightProxy::getProperty( Value& targetValue, const void* pInstance, const char* pName ) const
	{
		const Light* pTypedInstance = static_cast< const Light* >( pInstance );

		if( isStringEquals( pName, "On" ) )
		{
			targetValue.setBoolean( pTypedInstance->getOn() );
			return true;
		}

		return false;
	}

	bool LightProxy::setProperty( void* pInstance, const char* pName, const Value& value )
	{
		Light* pTypedInstance = static_cast< Light* >( pInstance );

		if( isStringEquals( pName, "On" ) )
		{
			pTypedInstance->setOn( value.getBoolean() );
			return true;
		}

		return false;
	}

	bool LightProxy::callFunction( Value& returnValue, void* pInstance, const char* pName, const Array< Value >& parameters )
	{
		const Light* pTypedInstance = static_cast< Light* >( pInstance );


		return false;
	}
}
