#include "home_dimmer_proxy.h"

#include "conct_string.h"

#include "home_dimmer.h"

namespace conct
{
	DimmerProxy::DimmerProxy()
		: Proxy( Dimmer::s_typeCrc )
	{
	}

	bool DimmerProxy::getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, const char* pName ) const
	{
		const Dimmer* pTypedInstance = static_cast< const Dimmer* >( pInstance );

		if( isStringEquals( pName, "Brightness" ) )
		{
			targetValueBuilder.setPercentValue( pTypedInstance->getBrightness() );
			return true;
		}

		return false;
	}

	bool DimmerProxy::setProperty( void* pInstance, const char* pName, const Value& value )
	{
		Dimmer* pTypedInstance = static_cast< Dimmer* >( pInstance );

		if( isStringEquals( pName, "Brightness" ) )
		{
			pTypedInstance->setBrightness( value.getPercentValue() );
			return true;
		}

		return false;
	}

	bool DimmerProxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, const char* pName, const ArrayView< Value >& parameters )
	{
		const Dimmer* pTypedInstance = static_cast< Dimmer* >( pInstance );


		return false;
	}
}
