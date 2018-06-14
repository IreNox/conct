#pragma once

#include "conct_core.h"

namespace conct
{
	class Timer
	{
	public:

					Timer();

		void		reset();

		double		getElapsedSeconds()	const;

	private:

		double		m_startTime;

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		uint64		m_frequence;
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
#elif CONCT_ENABLED( CONCT_PLATFORM_AVR )
#else
#	error "Platform not supported"
#endif

		double		getCurrentTime() const;
	};
}
