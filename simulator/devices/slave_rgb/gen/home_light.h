#pragma once

#include "conct_core.h"

namespace conct
{
	class Light
	{
	public:

		Boolean getOn() const = 0;
		void setOn( Boolean value ) = 0;
	};
}
