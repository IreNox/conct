#include "RNG.h"

#include <Arduino.h>
#include "Entropy.h"

void RNGClass::rand(uint8_t *data, size_t len)
{
	while( len-- )
	{
		*data = Entropy.randomByte();
		data++;
	}
}
