#pragma once

#include "gen/home_rgb.h"

namespace conct
{
	class RGBImpl : RGB
	{
	public:

		RGBImpl();

		virtual PercentValue getRed() const CONCT_OVERRIDE_FINAL;
		virtual void setRed( PercentValue value ) CONCT_OVERRIDE_FINAL;
		virtual PercentValue getGreen() const CONCT_OVERRIDE_FINAL;
		virtual void setGreen( PercentValue value ) CONCT_OVERRIDE_FINAL;
		virtual PercentValue getBlue() const CONCT_OVERRIDE_FINAL;
		virtual void setBlue( PercentValue value ) CONCT_OVERRIDE_FINAL;
	};
}
