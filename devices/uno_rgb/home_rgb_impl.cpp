#include "home_rgb_impl.h"

#include "light.h"

namespace conct
{
	PercentValue RGBImpl::getRed() const
	{
		return s_light.getRed();
	}

	void RGBImpl::setRed( PercentValue value )
	{
		s_light.setRed( value );
	}

	PercentValue RGBImpl::getGreen() const
	{
		return s_light.getGreen();
	}

	void RGBImpl::setGreen( PercentValue value )
	{
		s_light.setGreen( value );
	}

	PercentValue RGBImpl::getBlue() const
	{
		return s_light.getBlue();
	}

	void RGBImpl::setBlue( PercentValue value )
	{
		s_light.setBlue( value );
	}
}
