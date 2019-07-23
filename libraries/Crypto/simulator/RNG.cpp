#include "RNG.h"

#include "conct_crypto_random.h"

RNGClass RNG;

void RNGClass::rand( uint8_t *data, size_t len )
{
	conct::crypto::calculateRandomBytes( data, len );
}
