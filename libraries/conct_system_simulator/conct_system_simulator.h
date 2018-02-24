#pragma once

#include "simulator_device.h"

#include "conct_runtime.h"

namespace conct
{
	class SystemSimulator : public SimulatorDevice
	{
	public:

		virtual void setup() CONCT_OVERRIDE_FINAL;
		virtual void loop() CONCT_OVERRIDE_FINAL;

	private:

		Runtime	m_runtime;
	};
}
