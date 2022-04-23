#pragma once

#include "gen/device_interface.h"

namespace conct
{
	class DeviceRouterSimulator : public DeviceInterface
	{
	public:

		RuntimeHigh&	getRuntime() { return m_runtime; }
		Router&			getRouter() { return m_instanceRouter; }

	protected:

		virtual void setup() CONCT_OVERRIDE_FINAL;
		virtual void loop() CONCT_OVERRIDE_FINAL;
	};
}
