#pragma once

#include "gen/device_interface.h"

namespace conct
{
	class DeviceController : public DeviceInterface
	{
	public:

		Runtime&		getRuntime() { return m_runtime; }

	protected:

		virtual void	setup() TIKI_OVERRIDE_FINAL;
		virtual void	loop() TIKI_OVERRIDE_FINAL;
	};
}
