#include "home_rgb_proxy.h"

#include "home_rgb.h"
#include "conct_core.h"

namespace conct
{
	RGBProxy::RGBProxy()
		: Proxy( RGB::s_typeCrc )
	{
	}

	bool RGBProxy::getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, uint16 nameCrc ) const
	{
		const RGB* pTypedInstance = static_cast< const RGB* >( pInstance );

		if( nameCrc == 0x6a8b )
		{
			targetValueBuilder.setPercentValue( pTypedInstance->getRed() );
			return true;
		}

		if( nameCrc == 0xd415 )
		{
			targetValueBuilder.setPercentValue( pTypedInstance->getGreen() );
			return true;
		}

		if( nameCrc == 0xde32 )
		{
			targetValueBuilder.setPercentValue( pTypedInstance->getBlue() );
			return true;
		}

		return false;
	}

	bool RGBProxy::setProperty( void* pInstance, uint16 nameCrc, const Value& value ) const
	{
		RGB* pTypedInstance = static_cast< RGB* >( pInstance );

		if( nameCrc == 0x6a8b )
		{
			pTypedInstance->setRed( value.getPercentValue() );
			return true;
		}

		if( nameCrc == 0xd415 )
		{
			pTypedInstance->setGreen( value.getPercentValue() );
			return true;
		}

		if( nameCrc == 0xde32 )
		{
			pTypedInstance->setBlue( value.getPercentValue() );
			return true;
		}

		return false;
	}

	bool RGBProxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, uint16 nameCrc, const ArrayView< Value >& parameters ) const
	{
		return false;
	}
}
