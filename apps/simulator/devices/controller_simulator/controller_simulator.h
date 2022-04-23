#pragma once

#include "i_simulator_device.h"

#include "conct_controller.h"

#include "device_controller_simulator.h"

namespace conct
{
	class ControllerSimulator : public ISimulatorDevice
	{
	public:

									ControllerSimulator();
		virtual						~ControllerSimulator();

		virtual void				setup() CONCT_OVERRIDE_FINAL;
		virtual void				loop() CONCT_OVERRIDE_FINAL;

		virtual void 				fillData( SimulatorDeviceData& targetData ) CONCT_OVERRIDE_FINAL;

	private:

		DeviceControllerSimulator	m_device;
		Controller					m_controller;
	};
}