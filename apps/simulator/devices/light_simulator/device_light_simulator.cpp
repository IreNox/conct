#include "device_light_simulator.h"

namespace conct
{
	void DeviceLightSimulator::setup()
	{
		m_instanceLight.setControl( &m_lightControl );
		m_instanceDimmer.setControl( &m_lightControl );
		m_instanceRGB.setControl( &m_lightControl );
	}

	void DeviceLightSimulator::loop()
	{
		// TODO: implement
	}
}
