#pragma once

#include "i_simulator_device.h"

#include "conct_controller.h"
#include "conct_router.h"

#include "device_controller_simulator.h"

namespace conct
{
	class ControllerSimulator : public ISimulatorDevice
	{
	public:

									ControllerSimulator();
		virtual						~ControllerSimulator();

		virtual void				setup() override final;
		virtual void				loop() override final;

		virtual void 				fillData( SimulatorDeviceData& targetData ) override final;

	private:

		DeviceControllerSimulator	m_device;
		Router						m_router;
		Controller					m_controller;
	};
}