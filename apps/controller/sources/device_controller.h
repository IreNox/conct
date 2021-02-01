#pragma once

#include "gen/device_interface.h"

namespace conct
{
	class DeviceController : public DeviceInterface
	{
	public:

		RuntimeHigh&	getRuntime() { return m_runtime; }

	protected:

		virtual void	setup() CONCT_OVERRIDE_FINAL;
		virtual void	loop() CONCT_OVERRIDE_FINAL;
	};
}
