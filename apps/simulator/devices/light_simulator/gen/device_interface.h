#pragma once

#include "conct_device.h"
#include "conct_runtime_low.h"

#include "conct_port_serial.h"

#include "core_device_proxy.h"
#include "home_light_proxy.h"
#include "home_dimmer_proxy.h"
#include "home_rgb_proxy.h"

#include "../home_light_impl.h"
#include "../home_dimmer_impl.h"
#include "../home_rgb_impl.h"

namespace conct
{
	class DeviceInterface : public Device
	{
	public:

		DeviceInterface();

		void setupDevice();
		void loopDevice();

		virtual const char* getName() const CONCT_OVERRIDE_FINAL;

	protected:

		RuntimeLow m_runtime;

		PortSerial m_port0;

		DeviceProxy m_proxyDevice;
		LightProxy m_proxyLight;
		DimmerProxy m_proxyDimmer;
		RGBProxy m_proxyRGB;

		LightImpl m_instanceLight;
		DimmerImpl m_instanceDimmer;
		RGBImpl m_instanceRGB;

		virtual void setup() = 0;
		virtual void loop() = 0;

		virtual void getEmptyInstances( Array< Instance >& instances ) CONCT_OVERRIDE_FINAL;
		virtual void getPublicInstances( ArrayView< Instance >& instances ) const CONCT_OVERRIDE_FINAL;
		virtual void getLocalInstances( ArrayView< LocalInstance >& instances ) CONCT_OVERRIDE_FINAL;
	};
}
