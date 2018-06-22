#include "conct_port_serial_sim.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )

#include "i_simulator_context.h"
#include "i_simulator_device_context.h"

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
		return getSimulatorContext().getCurrentDevice()->getAvailableSerialData();
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
		return getSimulatorContext().getCurrentDevice()->readSerialData( pBuffer, bufferLength );
	}

	void Serial::write( uint8 data )
	{
		ISimulatorDeviceContext* pMyDevice = getSimulatorContext().getCurrentDevice();
		ISimulatorDeviceContext* pOpposideDevice = getSimulatorContext().getDevice( 1u - pMyDevice->getDeviceId() );
		if( pOpposideDevice == nullptr )
		{
			return;
		}

		pOpposideDevice->writeSerialData( data );
	}
}

#endif