#pragma once

#include "conct_core.h"

namespace conct
{
	class LightControl
	{
	public:

						LightControl();

		bool			getOn() const;
		void			setOn( bool  value );

		PercentValue	getBrightness() const;
		void			setBrightness( PercentValue value );

		PercentValue	getRed() const;
		void			setRed( PercentValue value );
		PercentValue	getGreen() const;
		void			setGreen( PercentValue value );
		PercentValue	getBlue() const;
		void			setBlue( PercentValue value );

	private:

		bool			m_on;

		PercentValue	m_brightness;

		PercentValue	m_red;
		PercentValue	m_green;
		PercentValue	m_blue;

		PercentValue	getColorValue( PercentValue sourceValue ) const;
		void			setColorValue( PercentValue& targetValue, PercentValue sourceValue );
	};
}