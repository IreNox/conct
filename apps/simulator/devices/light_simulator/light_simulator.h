#pragma once

#include "i_simulator_device.h"

#include "device_light_simulator.h"

namespace conct
{
	class LightSimulator : public ISimulatorDevice
	{
	public:

								LightSimulator();
		virtual					~LightSimulator();

		virtual void			setup() CONCT_OVERRIDE_FINAL;
		virtual void			loop() CONCT_OVERRIDE_FINAL;

		virtual void 			fillData( SimulatorDeviceData& targetData ) CONCT_OVERRIDE_FINAL;

	private:

		DeviceLightSimulator	m_device;
	};
}