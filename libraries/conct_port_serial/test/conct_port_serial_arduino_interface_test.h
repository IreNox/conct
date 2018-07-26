#pragma once

#include "conct_core.h"
#include "conct_timer.h"

#if CONCT_ENABLED( CONCT_ENVIRONMENT_TESTS )

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
		ISimulatorDeviceContext* pContext = getSimulatorContext().getCurrentDevice();
		if( pContext != nullptr )
		{
			return pContext->getGpio( pin ) ? HIGH : LOW;
		}

		return LOW;
	}

	CONCT_FORCE_INLINE void digitalWrite( uintreg pin, PinValue value )
	{
		ISimulatorDeviceContext* pContext = getSimulatorContext().getCurrentDevice();
		if( pContext != nullptr )
		{
			pContext->setGpio( pin, value == HIGH );
		}
	}

	CONCT_FORCE_INLINE uint32 millis()
	{
		static Timer s_timer;
		return uint32( s_timer.getElapsedSeconds() * 1000.0 );
	}
}

#endif
