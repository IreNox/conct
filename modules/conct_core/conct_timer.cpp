#include "conct_timer.h"

#if TIKI_ENABLED( TIKI_PLATFORM_WINDOWS )
#	include <windows.h>
#elif TIKI_ENABLED( TIKI_PLATFORM_LINUX ) || TIKI_ENABLED( TIKI_PLATFORM_ANDROID ) || TIKI_ENABLED( TIKI_PLATFORM_ESP )
#	include <time.h>
#endif

namespace conct
{
	Timer::Timer()
	{
#if TIKI_ENABLED( TIKI_PLATFORM_WINDOWS )
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
#if TIKI_ENABLED( TIKI_PLATFORM_WINDOWS )
		LARGE_INTEGER currentCounter;
		QueryPerformanceCounter( ( LARGE_INTEGER* )&currentCounter );
		return double( currentCounter.QuadPart ) / m_frequence;
#elif TIKI_ENABLED( TIKI_PLATFORM_LINUX ) || TIKI_ENABLED( TIKI_PLATFORM_ANDROID ) || TIKI_ENABLED( TIKI_PLATFORM_ESP )
		timespec currentTime;
		currentTime.tv_sec = 0;
		currentTime.tv_nsec = 0;
		clock_gettime( CLOCK_MONOTONIC, &currentTime );

		const double seconds = double( currentTime.tv_sec );
		const double nanoseconds = double( currentTime.tv_nsec );
		return seconds + ( nanoseconds / 1000000000.0 );
#else
#	error "Platform not supported"
#endif
	}
}