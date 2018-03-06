#include "home_dimmer_proxy.h"

#include "home_dimmer.h"

namespace conct
{
	DimmerProxy::DimmerProxy()
		: Proxy( 0u )
	{
	}

	bool DimmerProxy::getProperty( Value& targetValue, const void* pInstance, const char* pName ) const
	{
		const Dimmer* pTypedInstance = static_cast< const Dimmer* >( pInstance );

		if( isStringEqual( pName, "Brightness" ) )
		{
			targetValue.setPercentValue( pTypedInstance->getBrightness() );
			return true;
		}

		return false;
	}

	bool DimmerProxy::setProperty( void* pInstance, const char* pName, const Value& value )
	{
		Dimmer* pTypedInstance = static_cast< Dimmer* >( pInstance );

		if( isStringEqual( pName, "Brightness" ) )
		{
			pTypedInstance->setBrightness( value.getPercentValue() );
			return true;
		}

		return false;
	}

	bool DimmerProxy::callFunction( Value& returnValue, void* pInstance, const char* pName, const Array< Value >& parameters )
	{
		const Dimmer* pTypedInstance = static_cast< Dimmer* >( pInstance );


		return false;
	}
}
