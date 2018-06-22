#pragma once

#include "i_simulator_device.h"

#include "conct_controller.h"

#include "device_impl.h"

namespace conct
{
	class RouterSimulator : public ISimulatorDevice
	{
	public:
								RouterSimulator();
		virtual					~RouterSimulator();

		virtual void			setup() CONCT_OVERRIDE_FINAL;
		virtual void			loop() CONCT_OVERRIDE_FINAL;

		virtual void			fillData( SimulatorDeviceData& targetData ) CONCT_OVERRIDE_FINAL;

	private:

		DeviceRouterSimulator	m_device;
		Controller				m_controller;
	};
}
