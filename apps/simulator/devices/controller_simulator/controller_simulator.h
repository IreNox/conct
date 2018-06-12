#pragma once

#include "../../include/simulator_device.h"

#include "conct_controller.h"

#include "device_impl.h"

namespace conct
{
	class ControllerSimulator : public SimulatorDevice
	{
	public:

									ControllerSimulator();
		virtual						~ControllerSimulator();

		virtual void				setup() override final;
		virtual void				loop() override final;

		virtual void 				fillData( SimulatorDeviceData& targetData ) override final;

	private:

		DeviceControllerSimulator	m_device;
		Controller					m_controller;
	};
}