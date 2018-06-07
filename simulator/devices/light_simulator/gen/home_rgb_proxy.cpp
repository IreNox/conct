#include "home_rgb_proxy.h"

#include "conct_string.h"

#include "home_rgb.h"

namespace conct
{
	RGBProxy::RGBProxy()
		: Proxy( RGB::s_typeCrc )
	{
	}

	bool RGBProxy::getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, const char* pName ) const
	{
		const RGB* pTypedInstance = static_cast< const RGB* >( pInstance );

		if( isStringEquals( pName, "Red" ) )
		{
			targetValueBuilder.setPercentValue( pTypedInstance->getRed() );
			return true;
		}
		if( isStringEquals( pName, "Green" ) )
		{
			targetValueBuilder.setPercentValue( pTypedInstance->getGreen() );
			return true;
		}
		if( isStringEquals( pName, "Blue" ) )
		{
			targetValueBuilder.setPercentValue( pTypedInstance->getBlue() );
			return true;
		}

		return false;
	}

	bool RGBProxy::setProperty( void* pInstance, const char* pName, const Value& value )
	{
		RGB* pTypedInstance = static_cast< RGB* >( pInstance );

		if( isStringEquals( pName, "Red" ) )
		{
			pTypedInstance->setRed( value.getPercentValue() );
			return true;
		}
		if( isStringEquals( pName, "Green" ) )
		{
			pTypedInstance->setGreen( value.getPercentValue() );
			return true;
		}
		if( isStringEquals( pName, "Blue" ) )
		{
			pTypedInstance->setBlue( value.getPercentValue() );
			return true;
		}

		return false;
	}

	bool RGBProxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, const char* pName, const ArrayView< Value >& parameters )
	{
		const RGB* pTypedInstance = static_cast< RGB* >( pInstance );


		return false;
	}
}
