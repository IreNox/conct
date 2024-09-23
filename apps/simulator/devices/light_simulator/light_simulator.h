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

		virtual void			setup() override final;
		virtual void			loop() override final;

		virtual void 			fillData( SimulatorDeviceData& targetData ) override final;

	private:

		DeviceLightSimulator	m_device;
	};
}