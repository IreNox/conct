#include "device_mega2560_rgb.h"
#include "gen/files.h"
#include "conct_light_control.h"

static const uint8_t Red = 2;
static const uint8_t Green = 3;
static const uint8_t Blue = 4;

conct::Devicemega2560_rgb s_device;
conct::LightControl s_light;

void setup()
{
	s_device.setupDevice();
  
	pinMode(Red, OUTPUT);
	pinMode(Green, OUTPUT);
	pinMode(Blue, OUTPUT);

	analogWrite( Red, 0 );
	analogWrite( Green, 0 );
	analogWrite( Blue, 0 );
}

void loop()
{
	s_device.loopDevice();

	analogWrite( Red, s_light.getRed() / 257 );
	analogWrite( Green, s_light.getGreen() / 257 );
	analogWrite( Blue, s_light.getBlue() / 257 );

	delay(10);
}
