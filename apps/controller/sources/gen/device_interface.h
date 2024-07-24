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

		virtual const char* getName() const TIKI_OVERRIDE_FINAL;

	protected:

		Runtime m_runtime;


		DeviceProxy m_proxyDevice;


		virtual void setup() = 0;
		virtual void loop() = 0;

		virtual void getEmptyInstances( ArrayView< Instance >& instances ) TIKI_OVERRIDE_FINAL;
		virtual void getPublicInstances( ConstInstanceView& instances ) const TIKI_OVERRIDE_FINAL;
		virtual void getLocalInstances( ArrayView< const LocalInstance >& instances ) TIKI_OVERRIDE_FINAL;
	};
}
