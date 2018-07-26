#pragma once

#include "conct_core.h"

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )

#include <tiny_gpio/tiny_gpio.h>

#include <time.h>

namespace conct
{
	enum PinMode
	{
		INPUT,
		OUTPUT
	};

	enum PinValue
	{
		LOW,
		HIGH
	};

	CONCT_FORCE_INLINE void pinMode( uintreg pin, PinMode mode )
	{
		gpioSetMode( pin, mode );
	}

	CONCT_FORCE_INLINE PinValue digitalRead( uintreg pin )
	{
		return gpioRead( pin ) ? HIGH : LOW;
	}

	CONCT_FORCE_INLINE void digitalWrite( uintreg pin, PinValue value )
	{
		gpioWrite( pin, value );
	}

	CONCT_FORCE_INLINE uint32 millis()
	{
		timespec currentTime;
		currentTime.tv_sec = 0;
		currentTime.tv_nsec = 0;
		clock_gettime( CLOCK_MONOTONIC, &currentTime );

		uint32 result = uint32( currentTime.tv_sec ) * 1000u;
		result += uint32( currentTime.tv_nsec ) / 1000000u;
		return result;
	}
}

#endif
