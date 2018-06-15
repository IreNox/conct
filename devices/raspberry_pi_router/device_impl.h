#pragma once

#include "gen/device_interface.h"

namespace conct
{
	class DeviceRaspiRouter : public DeviceInterface
	{
	protected:

		virtual void setup() CONCT_OVERRIDE_FINAL;
		virtual void loop() CONCT_OVERRIDE_FINAL;
	};
}
