#pragma once

#include "i_simulator_device.h"

#include "conct_controller.h"

#include "device_router_simulator.h"

namespace conct
{
	class RouterSimulator : public ISimulatorDevice
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
