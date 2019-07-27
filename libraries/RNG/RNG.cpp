#include "RNG.h"

#include "Entropy.h"

void RNGClass::rand(uint8_t *data, size_t len)
{
	for( uint8_t i = 0u; i < len; ++len)
	{
		data[ i ] = Entropy.randomByte();
	}
}
