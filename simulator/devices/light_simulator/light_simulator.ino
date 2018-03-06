#include "device_impl.h"
#include "gen/files.h"

conct::DeviceLightSimulator device;

void setup()
{
	device.setupDevice();
}

void loop()
{
	device.loopDevice();
}

