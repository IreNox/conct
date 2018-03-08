#pragma once

#include "conct_device.h"
#include "conct_runtime_high.h"

#include "conct_port_lan.h"
#include "conct_port_rs485_sim.h"



namespace conct
{
	class DeviceInterface : public Device
	{
	public:

		void setupDevice();
		void loopDevice();

		virtual void getInstances( ArrayView< LocalInstance >& instances ) CONCT_OVERRIDE_FINAL;

	protected:

		virtual void setup() = 0;
		virtual void loop() = 0;

	private:

		RuntimeHigh m_runtime;

		PortLan m_port0;
		PortRs485Sim m_port1;


	};
}
