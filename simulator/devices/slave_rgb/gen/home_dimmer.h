#pragma once

#include "conct_core.h"

namespace conct
{
	class Dimmer
	{
	public:

		PercentValue getBrightness() const = 0;
		void setBrightness( PercentValue value ) = 0;
	};
}
