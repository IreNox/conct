#include "device_android_controller.h"

namespace conct
{
	void DeviceAndroidController::setup()
	{
		m_controller.setup( &m_runtime );
	}

	void DeviceAndroidController::loop()
	{
		m_controller.loop();
	}
}
