#pragma once

#include "conct_core.h"

namespace conct
{
	class RGB
	{
	public:

		PercentValue getRed() const = 0;
		void setRed( PercentValue value ) = 0;
		PercentValue getGreen() const = 0;
		void setGreen( PercentValue value ) = 0;
		PercentValue getBlue() const = 0;
		void setBlue( PercentValue value ) = 0;
	};
}
