#pragma once

#include "conct_device.h"
#include "conct_runtime_high.h"

#include "conct_port_tcp.h"
#include "conct_port_rs485_sim.h"

#include "core_device_proxy.h"
#include "core_router_proxy.h"

#include "conct_router.h"

namespace conct
{
	class DeviceInterface : public Device
	{
	public:

		void setupDevice();
		void loopDevice();

		virtual const char* getName() const CONCT_OVERRIDE_FINAL;

	protected:

		RuntimeHigh m_runtime;

		virtual void setup() = 0;
		virtual void loop() = 0;

		virtual void getEmptyInstances( Array< Instance >& instances ) CONCT_OVERRIDE_FINAL;
		virtual void getPublicInstances( ArrayView< Instance >& instances ) const CONCT_OVERRIDE_FINAL;
		virtual void getLocalInstances( ArrayView< LocalInstance >& instances ) CONCT_OVERRIDE_FINAL;

	private:

		PortTcp m_port0;
		PortRs485Sim m_port1;

		DeviceProxy m_proxyDevice;
		RouterProxy m_proxyRouter;

		Router m_instanceRouter;
	};
}
