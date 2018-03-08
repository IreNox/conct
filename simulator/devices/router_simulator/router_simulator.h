#pragma once

#include "../../include/simulator_device.h"

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

	private:

		DeviceRouterSimulator	m_device;
	};
}
