#include "device_uno_rgb.h"
#include "gen/files.h"

#include "conct_light_control.h"

conct::Deviceuno_rgb device;
conct::LightControl s_light;

void setup()
{
  Serial.begin( 9600 );
  
	device.setupDevice();
}

void loop()
{
	device.loopDevice();
}
