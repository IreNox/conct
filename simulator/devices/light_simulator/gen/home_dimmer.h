#pragma once

#include "conct_core.h"

namespace conct
{
	class Dimmer
	{
	public:

		virtual PercentValue getBrightness() const = 0;
		virtual void setBrightness( PercentValue value ) = 0;
	};
}
