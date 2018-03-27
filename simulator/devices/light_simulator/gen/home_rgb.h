#pragma once

#include "conct_core.h"

namespace conct
{
	class RGB
	{
	public:

		static const TypeCrc s_typeCrc = 7621;

		virtual PercentValue getRed() const = 0;
		virtual void setRed( PercentValue value ) = 0;
		virtual PercentValue getGreen() const = 0;
		virtual void setGreen( PercentValue value ) = 0;
		virtual PercentValue getBlue() const = 0;
		virtual void setBlue( PercentValue value ) = 0;
	};
}
