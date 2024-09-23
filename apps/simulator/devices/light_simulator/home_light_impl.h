#pragma once

#include "gen/home_light.h"

namespace conct
{
	class LightImpl : Light
	{
	public:

		virtual LightSource getSource() const TIKI_OVERRIDE_FINAL;
		virtual bool getOn() const TIKI_OVERRIDE_FINAL;
		virtual void setOn( bool value ) TIKI_OVERRIDE_FINAL;
		virtual PercentValue getBrightness() const TIKI_OVERRIDE_FINAL;
		virtual void setBrightness( PercentValue value ) TIKI_OVERRIDE_FINAL;
		virtual PercentValue getRed() const TIKI_OVERRIDE_FINAL;
		virtual void setRed( PercentValue value ) TIKI_OVERRIDE_FINAL;
		virtual PercentValue getGreen() const TIKI_OVERRIDE_FINAL;
		virtual void setGreen( PercentValue value ) TIKI_OVERRIDE_FINAL;
		virtual PercentValue getBlue() const TIKI_OVERRIDE_FINAL;
		virtual void setBlue( PercentValue value ) TIKI_OVERRIDE_FINAL;
		virtual PercentValue getWhite() const TIKI_OVERRIDE_FINAL;
		virtual void setWhite( PercentValue value ) TIKI_OVERRIDE_FINAL;
		virtual PercentValue getWhiteColdWarm() const TIKI_OVERRIDE_FINAL;
		virtual void setWhiteColdWarm( PercentValue value ) TIKI_OVERRIDE_FINAL;
	};
}
