#pragma once

#include "conct_core.h"

namespace conct
{
	class Timer
	{
	public:

				Timer();

		void	reset();

		double	getElapsedSeconds()	const;

	private:

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		uint64	m_startTime;
		uint64	m_frequence;
#else
#	error "Platform not supported"
#endif
	};
}