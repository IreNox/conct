#include "home_dimmer_proxy.h"

#include "home_dimmer.h"
#include "conct_core.h"

namespace conct
{
	DimmerProxy::DimmerProxy()
		: Proxy( Dimmer::s_typeCrc )
	{
	}

	bool DimmerProxy::getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, uint16 nameCrc ) const
	{
		const Dimmer* pTypedInstance = static_cast< const Dimmer* >( pInstance );

		if( nameCrc == 0xf9df )
		{
			targetValueBuilder.setPercentValue( pTypedInstance->getBrightness() );
			return true;
		}

		return false;
	}

	bool DimmerProxy::setProperty( void* pInstance, uint16 nameCrc, const Value& value ) const
	{
		Dimmer* pTypedInstance = static_cast< Dimmer* >( pInstance );

		if( nameCrc == 0xf9df )
		{
			pTypedInstance->setBrightness( value.getPercentValue() );
			return true;
		}

		return false;
	}

	bool DimmerProxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, uint16 nameCrc, const ArrayView< Value >& parameters ) const
	{
		return false;
	}
}
