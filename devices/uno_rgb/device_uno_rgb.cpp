#include "device_uno_rgb.h"

#include "light.h"

namespace conct
{
	static const uint8 Red = 3;
	static const uint8 Green = 5;
	static const uint8 Blue = 6;

	void Deviceuno_rgb::setup()
	{
		pinMode( Red, OUTPUT );
		pinMode( Green, OUTPUT );
		pinMode( Blue, OUTPUT );

		analogWrite( Red, 0 );
		analogWrite( Green, 0 );
		analogWrite( Blue, 0 );
	}

	void Deviceuno_rgb::loop()
	{
		//Serial.print( s_light.getRed() );
		//Serial.print( '/' );
		//Serial.print( s_light.getGreen() );
		//Serial.print( '/' );
		//Serial.println( s_light.getBlue() );

		analogWrite( Red, s_light.getRed() / 257 );
		analogWrite( Green, s_light.getGreen() / 257 );
		analogWrite( Blue, s_light.getBlue() / 257 );

		delay( 10 );
	}
}
