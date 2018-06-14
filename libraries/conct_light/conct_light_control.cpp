#include "conct_light_control.h"

namespace conct
{
	LightControl::LightControl()
	{
		m_on			= true;
		m_brightness	= PercentValueFull;
		m_red			= PercentValueFull;
		m_green			= PercentValueFull;
		m_blue			= PercentValueFull;
	}

	bool LightControl::getOn() const
	{
		return m_on;
	}

	void LightControl::setOn( bool value )
	{
		m_on = value;
	}

	PercentValue LightControl::getBrightness() const
	{
		return m_brightness;
	}

	void LightControl::setBrightness( PercentValue value )
	{
		m_on			= value > 0u;
		m_brightness	= value;
	}

	PercentValue LightControl::getRed() const
	{
		return getColorValue( m_red );
	}

	void LightControl::setRed( PercentValue value )
	{
		setColorValue( m_red, value );
	}

	PercentValue LightControl::getGreen() const
	{
		return getColorValue( m_green );
	}

	void LightControl::setGreen( PercentValue value )
	{
		setColorValue( m_green, value );
	}

	PercentValue LightControl::getBlue() const
	{
		return getColorValue( m_blue );
	}

	void LightControl::setBlue( PercentValue value )
	{
		setColorValue( m_blue, value );
	}

	PercentValue LightControl::getColorValue( PercentValue sourceValue ) const
	{
		if( !m_on )
		{
			return 0u;
		}

		uint32 value = sourceValue;
		value *= m_brightness;
		value /= PercentValueFull;
		return PercentValue( value );
	}

	void LightControl::setColorValue( PercentValue& targetValue, PercentValue sourceValue )
	{
		m_on = m_red > 0u || m_green > 0u || m_blue > 0u;

		if( m_on )
		{
			m_brightness = PercentValueFull;
		}

		targetValue = sourceValue;
	}
}
