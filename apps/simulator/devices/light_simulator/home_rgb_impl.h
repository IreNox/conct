#pragma once

#include "gen/home_rgb.h"

namespace conct
{
	class LightControl;

	class RGBImpl : RGB
	{
	public:

		void setControl( LightControl* pLightControl ) { m_pLightControl = pLightControl; }

		virtual PercentValue getRed() const CONCT_OVERRIDE_FINAL;
		virtual void setRed( PercentValue value ) CONCT_OVERRIDE_FINAL;
		virtual PercentValue getGreen() const CONCT_OVERRIDE_FINAL;
		virtual void setGreen( PercentValue value ) CONCT_OVERRIDE_FINAL;
		virtual PercentValue getBlue() const CONCT_OVERRIDE_FINAL;
		virtual void setBlue( PercentValue value ) CONCT_OVERRIDE_FINAL;

	private:

		LightControl * m_pLightControl;
	};
}
