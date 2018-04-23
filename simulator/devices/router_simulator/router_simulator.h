#pragma once

#include "../../include/simulator_device.h"

#include "conct_controller.h"
#include "device_impl.h"

namespace conct
{
	class RouterSimulator : public SimulatorDevice
	{
	public:
								RouterSimulator();
		virtual					~RouterSimulator();

		virtual void			setup() override final;
		virtual void			loop() override final;

		virtual void			fillData( SimulatorDeviceData& targetData ) override final;

	private:

		DeviceRouterSimulator	m_device;
		Controller				m_controller;
	};
}
