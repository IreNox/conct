#pragma once

#include "conct_device.h"
#include "conct_runtime.h"


#include "core_device_proxy.h"


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


		DeviceProxy m_proxyDevice;


		virtual void setup() = 0;
		virtual void loop() = 0;

		virtual void getEmptyInstances( ArrayView< Instance >& instances ) override final;
		virtual void getPublicInstances( ConstInstanceView& instances ) const override final;
		virtual void getLocalInstances( ArrayView< const LocalInstance >& instances ) override final;
	};
}
