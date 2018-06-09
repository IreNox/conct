#pragma once

#include "conct_device.h"
#include "conct_runtime_low.h"

#include "conct_port_rs485_sim.h"

#include "home_rgb_proxy.h"
#include "core_device_proxy.h"
#include "home_dimmer_proxy.h"
#include "home_light_proxy.h"

#include "../home_rgb_impl.h"
#include "../home_dimmer_impl.h"
#include "../home_light_impl.h"

namespace conct
{
	class DeviceInterface : public Device
	{
	public:

		void setupDevice();
		void loopDevice();

		virtual const char* getName() const CONCT_OVERRIDE_FINAL;

	protected:

		RuntimeLow m_runtime;

		virtual void setup() = 0;
		virtual void loop() = 0;

		virtual void getEmptyInstances( Array< Instance >& instances ) CONCT_OVERRIDE_FINAL;
		virtual void getPublicInstances( ArrayView< Instance >& instances ) const CONCT_OVERRIDE_FINAL;
		virtual void getLocalInstances( ArrayView< LocalInstance >& instances ) CONCT_OVERRIDE_FINAL;

	private:

		PortRs485Sim m_port0;

		RGBProxy m_proxyRGB;
		DeviceProxy m_proxyDevice;
		DimmerProxy m_proxyDimmer;
		LightProxy m_proxyLight;

		LightImpl m_instanceLight;
		DimmerImpl m_instanceDimmer;
		RGBImpl m_instanceRGB;
	};
}
