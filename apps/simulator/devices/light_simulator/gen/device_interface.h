#pragma once

#include "conct_device.h"
#include "conct_runtime.h"

#include "conct_port_tcp_client.h"

#include "core_device_proxy.h"
#include "home_light_proxy.h"

#include "../home_light_impl.h"

namespace conct
{
	class DeviceInterface : public Device
	{
	public:

		DeviceInterface();

		void setupDevice();
		void loopDevice();

		virtual const char* getName() const override final;

	protected:

		Runtime m_runtime;

		PortTcpClient m_port0;

		DeviceProxy m_proxyDevice;
		LightProxy m_proxyLight;

		LightImpl m_instanceLight;

		virtual void setup() = 0;
		virtual void loop() = 0;

		virtual void getEmptyInstances( ArrayView< Instance >& instances ) override final;
		virtual void getPublicInstances( ConstInstanceView& instances ) const override final;
		virtual void getLocalInstances( ArrayView< const LocalInstance >& instances ) override final;
	};
}
