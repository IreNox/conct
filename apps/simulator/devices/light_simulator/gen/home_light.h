#pragma once

#include "conct_core.h"
#include "home_lightsource.h"

namespace conct
{
	class Light
	{
	public:

		static const TypeCrc s_typeCrc = 0x5f0c;

		virtual LightSource getSource() const = 0;
		virtual bool getOn() const = 0;
		virtual void setOn( bool value ) = 0;
		virtual PercentValue getBrightness() const = 0;
		virtual void setBrightness( PercentValue value ) = 0;
		virtual PercentValue getRed() const = 0;
		virtual void setRed( PercentValue value ) = 0;
		virtual PercentValue getGreen() const = 0;
		virtual void setGreen( PercentValue value ) = 0;
		virtual PercentValue getBlue() const = 0;
		virtual void setBlue( PercentValue value ) = 0;
		virtual PercentValue getWhite() const = 0;
		virtual void setWhite( PercentValue value ) = 0;
		virtual PercentValue getWhiteColdWarm() const = 0;
		virtual void setWhiteColdWarm( PercentValue value ) = 0;
	};
}
