#pragma once

#include "core_device.h"
#include "conct_runtime_low.h"

#include "conct_port_rs485_sim.h"

#include "core_device_proxy.h"
#include "home_light_proxy.h"
#include "home_rgb_proxy.h"
#include "home_dimmer_proxy.h"

#include "../home_light_impl.h"
#include "../home_rgb_impl.h"
#include "../home_dimmer_impl.h"

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

		RuntimeLow m_runtime;

		PortRs485Sim m_port0;

		DeviceProxy m_proxyDevice;
		LightProxy m_proxyLight;
		RGBProxy m_proxyRGB;
		DimmerProxy m_proxyDimmer;

		LightImpl m_instanceLight;
		DimmerImpl m_instanceDimmer;
		RGBImpl m_instanceRGB;
	};
}
