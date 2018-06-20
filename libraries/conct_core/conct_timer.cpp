#include "conct_timer.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <windows.h>
#elif CONCT_ENABLED( CONCT_PLATFORM_LINUX ) || CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
#	include <time.h>
#elif CONCT_ENABLED( CONCT_PLATFORM_AVR )
#	include <arduino.h>
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
		m_startTime = getCurrentTime();
	}

	double Timer::getElapsedSeconds() const
	{
		return getCurrentTime() - m_startTime;
	}

	double Timer::getCurrentTime() const
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		LARGE_INTEGER currentCounter;
		QueryPerformanceCounter( ( LARGE_INTEGER* )&currentCounter );
		return double( currentCounter.QuadPart ) / m_frequence;
#elif CONCT_ENABLED( CONCT_PLATFORM_LINUX ) || CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
		timespec currentTime;
		currentTime.tv_sec = 0;
		currentTime.tv_nsec = 0;
		clock_gettime( CLOCK_MONOTONIC, &currentTime );

		const double seconds = double( currentTime.tv_sec );
		const double nanoseconds = double( currentTime.tv_nsec );
		return seconds + ( nanoseconds / 1000000000.0 );
#elif CONCT_ENABLED( CONCT_PLATFORM_AVR )
		return micros() / 1000000.0;
#endif
	}
}