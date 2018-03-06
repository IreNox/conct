#pragma once

#include "conct_core.h"

namespace conct
{
	class Light
	{
	public:

		virtual bool getOn() const = 0;
		virtual void setOn( bool value ) = 0;
	};
}
