#pragma once

#include "gen/device_interface.h"

namespace conct
{
	class DeviceRouterSimulator : public DeviceInterface
	{
	public:

		Runtime&	getRuntime() { return m_runtime; }
		Router&		getRouter() { return m_instanceRouter; }

	protected:

		virtual void setup() override final;
		virtual void loop() override final;
	};
}
