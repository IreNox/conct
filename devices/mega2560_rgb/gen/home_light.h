#pragma once

#include "conct_core.h"

namespace conct
{
	class Light
	{
	public:

		static const TypeCrc s_typeCrc = 24332;

		virtual bool getOn() const = 0;
		virtual void setOn( bool value ) = 0;
	};
}
