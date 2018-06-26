#include "home_light_impl.h"

#include "light.h"

namespace conct
{
	bool LightImpl::getOn() const
	{
		return s_light.getOn();
	}

	void LightImpl::setOn( bool value )
	{
		s_light.setOn( value );
	}
}
