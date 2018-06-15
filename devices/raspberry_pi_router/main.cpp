#include "device_impl.h"

int main(int argc, char *argv[])
{
	conct::DeviceRaspiRouter device;
	device.setupDevice();

	while( true )
	{
		device.loopDevice();
	}

	return 0;
}