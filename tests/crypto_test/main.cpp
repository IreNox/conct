#include "stdlib.h"
#include "windows.h"

#include <ChaCha.h>

int main()
{
	ChaCha c1;
	ChaCha c2;

	uint8_t key[ 32u ];
	for( int i = 0u; i < sizeof( key ); ++i )
	{
		key[ i ] = uint8_t( rand() );
	}
	c1.setKey( key, sizeof( key ) );
	c2.setKey( key, sizeof( key ) );

	uint8_t iv[ 12u ];
	for( int i = 0u; i < sizeof( iv ); ++i )
	{
		iv[ i ] = uint8_t( rand() );
	}
	c1.setIV( iv, sizeof( iv ) );
	c2.setIV( iv, sizeof( iv ) );

	const uint32_t counter = 1u;
	c1.setCounter( (const uint8_t*)&counter, sizeof( counter ) );
	c2.setCounter( (const uint8_t*)&counter, sizeof( counter ) );

	char text[] = "Hello world!";

	char buffer[ sizeof( text ) ];
	c1.encrypt( (uint8_t*)buffer, (const uint8_t*)text, sizeof( text ) );

	char buffer2[ sizeof( text ) ];
	c2.decrypt( (uint8_t*)buffer2, (const uint8_t*)buffer, sizeof( buffer ) );

	char buffer3[ sizeof( text ) ];
	c1.decrypt( (uint8_t*)buffer3, (const uint8_t*)buffer, sizeof( buffer ) );

	OutputDebugStringA( "\n" );
	OutputDebugStringA( buffer2 );
	OutputDebugStringA( "\n" );
	OutputDebugStringA( buffer3 );
	OutputDebugStringA( "\n" );

	return 0;
}
