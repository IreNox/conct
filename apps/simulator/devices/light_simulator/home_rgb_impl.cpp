#include "home_rgb_impl.h"

#include "conct_light_control.h"

namespace conct
{
	PercentValue RGBImpl::getRed() const
	{
		return m_pLightControl->getRed();
	}

	void RGBImpl::setRed( PercentValue value )
	{
		m_pLightControl->setRed( value );
	}

	PercentValue RGBImpl::getGreen() const
	{
		return m_pLightControl->getGreen();
	}

	void RGBImpl::setGreen( PercentValue value )
	{
		m_pLightControl->setGreen( value );
	}

	PercentValue RGBImpl::getBlue() const
	{
		return m_pLightControl->getBlue();
	}

	void RGBImpl::setBlue( PercentValue value )
	{
		m_pLightControl->setBlue( value );
	}
}
