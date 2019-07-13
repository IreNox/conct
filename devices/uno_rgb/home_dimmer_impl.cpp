#include "home_dimmer_impl.h"

#include "light.h"

namespace conct
{
	PercentValue DimmerImpl::getBrightness() const
	{
		return s_light.getBrightness();
	}

	void DimmerImpl::setBrightness( PercentValue value )
	{
		s_light.setBrightness( value );
	}
}
