#pragma once

#include "conct_core.h"

namespace conct
{
	class Dimmer
	{
	public:

		static const TypeCrc s_TypeCrc = 57793;

		virtual PercentValue getBrightness() const = 0;
		virtual void setBrightness( PercentValue value ) = 0;
	};
}
