#pragma once

#include "gen/home_dimmer.h"

namespace conct
{
	class DimmerImpl : Dimmer
	{
	public:

		virtual PercentValue getBrightness() const CONCT_OVERRIDE_FINAL;
		virtual void setBrightness( PercentValue value ) CONCT_OVERRIDE_FINAL;
	};
}
