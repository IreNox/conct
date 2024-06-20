#include "conct_simulator_device_context.h"

namespace conct
{
	SimulatorDeviceContext::SimulatorDeviceContext( uintreg deviceId )
	{
		m_deviceId = deviceId;
	}

	uintreg SimulatorDeviceContext::getDeviceId() const
	{
		return m_deviceId;
	}

	bool SimulatorDeviceContext::getGpio( uintreg pin ) const
	{
		const uint32 gpioValue = m_gpio.getRelaxed();
		return ( gpioValue & ( 1u << pin ) ) != 0u;
	}

	void SimulatorDeviceContext::setGpio( uintreg pin, bool value )
	{
		if( value )
		{
			const uint32 mask = ( 1u << pin );
			m_gpio.orRelaxed( mask );
		}
		else
		{
			const uint32 mask = ~( 1u << pin );
			m_gpio.andRelaxed( mask );
		}
	}
}