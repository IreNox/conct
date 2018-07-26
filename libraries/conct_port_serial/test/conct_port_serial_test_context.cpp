#include "conct_port_serial_test.h"

#if CONCT_ENABLED( CONCT_ENVIRONMENT_TESTS )

namespace conct
{
	bool Serial::begin( int speed )
	{
		return true;
	}

	void Serial::flush()
	{
	}

	int Serial::available()
	{
		return 0u;
	}

	int Serial::read()
	{
		uint8 byte;
		if( readData( &byte, sizeof( byte ) ) == 0u )
		{
			return -1;
		}

		return int( byte );
	}

	uintreg Serial::readData( void* pBuffer, uintreg bufferLength )
	{
		return 0u;
	}

	void Serial::write( uint8 data )
	{
	}
}

#endif