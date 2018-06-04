#include "conct_timer.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <windows.h>
#endif

namespace conct
{
	Timer::Timer()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		QueryPerformanceFrequency( ( LARGE_INTEGER* )&m_frequence );
#endif
		reset();
	}

	void Timer::reset()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		QueryPerformanceCounter( ( LARGE_INTEGER* )&m_startTime );
#endif

	}

	double Timer::getElapsedSeconds() const
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		LARGE_INTEGER currentCounter;
		QueryPerformanceCounter( ( LARGE_INTEGER* )&currentCounter );
		return double( currentCounter.QuadPart - m_startTime ) / m_frequence;
#endif
	}
}