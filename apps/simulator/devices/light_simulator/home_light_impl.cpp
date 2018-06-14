#include "home_light_impl.h"

#include "conct_light_control.h"

namespace conct
{
	bool LightImpl::getOn() const
	{
		return m_pLightControl->getOn();
	}

	void LightImpl::setOn( bool value )
	{
		m_pLightControl->setOn( value );
	}
}
