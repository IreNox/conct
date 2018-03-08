#pragma once

#include "../../include/simulator_device.h"

#include "device_impl.h"

namespace conct
{
	class LightSimulator : public SimulatorDevice
	{
	public:

								LightSimulator();
		virtual					~LightSimulator();

		virtual void			setup() override final;
		virtual void			loop() override final;

	private:

		DeviceLightSimulator	m_device;
	};
}