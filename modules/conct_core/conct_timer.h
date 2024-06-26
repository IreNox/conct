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

#if TIKI_ENABLED( TIKI_PLATFORM_WINDOWS )
		uint64		m_frequence;
#elif TIKI_ENABLED( TIKI_PLATFORM_LINUX ) || TIKI_ENABLED( TIKI_PLATFORM_ANDROID ) || TIKI_ENABLED( TIKI_PLATFORM_ESP )
#else
#	error "Platform not supported"
#endif

		double		getCurrentTime() const;
	};
}
