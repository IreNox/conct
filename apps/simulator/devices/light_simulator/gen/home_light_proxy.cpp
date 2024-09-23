#include "home_light_proxy.h"

#include "home_light.h"
#include "conct_core.h"
#include "home_lightsource.h"

namespace conct
{
	LightProxy::LightProxy()
		: Proxy( Light::s_typeCrc )
	{
	}

	bool LightProxy::getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, uint16 nameCrc ) const
	{
		const Light* pTypedInstance = static_cast< const Light* >( pInstance );

		if( nameCrc == 0xa68a )
		{
			targetValueBuilder.setLightSource( pTypedInstance->getSource() );
			return true;
		}

		if( nameCrc == 0xdcb5 )
		{
			targetValueBuilder.setBoolean( pTypedInstance->getOn() );
			return true;
		}

		if( nameCrc == 0xf9df )
		{
			targetValueBuilder.setPercentValue( pTypedInstance->getBrightness() );
			return true;
		}

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

		if( nameCrc == 0x9b5e )
		{
			targetValueBuilder.setPercentValue( pTypedInstance->getWhite() );
			return true;
		}

		if( nameCrc == 0x19a9 )
		{
			targetValueBuilder.setPercentValue( pTypedInstance->getWhiteColdWarm() );
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

		if( nameCrc == 0xf9df )
		{
			pTypedInstance->setBrightness( value.getPercentValue() );
			return true;
		}

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

		if( nameCrc == 0x9b5e )
		{
			pTypedInstance->setWhite( value.getPercentValue() );
			return true;
		}

		if( nameCrc == 0x19a9 )
		{
			pTypedInstance->setWhiteColdWarm( value.getPercentValue() );
			return true;
		}

		return false;
	}

	bool LightProxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, uint16 nameCrc, const ArrayView< const Value >& parameters ) const
	{
		return false;
	}
}
