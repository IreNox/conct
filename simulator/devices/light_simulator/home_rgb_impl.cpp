#include "home_rgb_impl.h"

namespace conct
{
	RGBImpl::RGBImpl()
	{

	}

	PercentValue RGBImpl::getRed() const
	{
		return 65535 / 2;
	}

	void RGBImpl::setRed( PercentValue value )
	{
		// TODO: implement
	}

	PercentValue RGBImpl::getGreen() const
	{
		return 65535 / 3;
	}

	void RGBImpl::setGreen( PercentValue value )
	{
		// TODO: implement
	}

	PercentValue RGBImpl::getBlue() const
	{
		return 65535;
	}

	void RGBImpl::setBlue( PercentValue value )
	{
		// TODO: implement
	}
}
