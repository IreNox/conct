#include "home_dimmer_impl.h"

#include "conct_light_control.h"

namespace conct
{
	PercentValue DimmerImpl::getBrightness() const
	{
		return m_pLightControl->getBrightness();
	}

	void DimmerImpl::setBrightness( PercentValue value )
	{
		m_pLightControl->setBrightness( value );
	}
}
