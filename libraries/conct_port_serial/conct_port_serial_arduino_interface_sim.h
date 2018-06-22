#pragma once

#include "conct_core.h"
#include "conct_timer.h"

#include "i_simulator_context.h"
#include "i_simulator_device_context.h"

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
	}

	CONCT_FORCE_INLINE PinValue digitalRead( uintreg pin )
	{
		return getSimulatorContext().getCurrentDevice()->getGpio( pin ) ? HIGH : LOW;
	}

	CONCT_FORCE_INLINE void digitalWrite( uintreg pin, PinValue value )
	{
		getSimulatorContext().getCurrentDevice()->setGpio( pin, value == HIGH );
	}

	CONCT_FORCE_INLINE uint32 millis()
	{
		static Timer s_timer;
		return uint32( s_timer.getElapsedSeconds() * 1000.0 );
	}
}
