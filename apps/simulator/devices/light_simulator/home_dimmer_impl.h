#pragma once

#include "gen/home_dimmer.h"

namespace conct
{
	class LightControl;

	class DimmerImpl : Dimmer
	{
	public:

		void setControl( LightControl* pLightControl ) { m_pLightControl = pLightControl; }

		virtual PercentValue getBrightness() const CONCT_OVERRIDE_FINAL;
		virtual void setBrightness( PercentValue value ) CONCT_OVERRIDE_FINAL;

	private:

		LightControl * m_pLightControl;
	};
}
