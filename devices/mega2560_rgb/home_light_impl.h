#pragma once

#include "gen/home_light.h"

namespace conct
{
	class LightControl;

	class LightImpl : Light
	{
	public:

		virtual bool getOn() const CONCT_OVERRIDE_FINAL;
		virtual void setOn( bool value ) CONCT_OVERRIDE_FINAL;
	};
}
