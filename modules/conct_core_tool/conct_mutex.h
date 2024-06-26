#pragma once

#include "conct_core.h"

namespace conct
{
	class Mutex
	{
	public:

				Mutex();
				~Mutex();

		void	lock() const;
		bool	tryLock() const;
		void	unlock() const;

	private:

				Mutex( const Mutex& );
		Mutex&	operator=( const Mutex& );

#if TIKI_ENABLED( TIKI_PLATFORM_WINDOWS)
#	if TIKI_ENABLED( TIKI_POINTER_64 )
		uint64	m_data[ 5u ];
#	elif TIKI_ENABLED( TIKI_POINTER_32 )
		uint32	m_data[ 6u ];
#	endif
#elif TIKI_ENABLED( TIKI_PLATFORM_ANDROID )
#	if TIKI_ENABLED( TIKI_POINTER_64 )
		sint32	m_data[ 10u ];
#	elif TIKI_ENABLED( TIKI_POINTER_32 )
		sint32	m_data[ 1u ];
#	endif
#elif TIKI_ENABLED( TIKI_PLATFORM_POSIX )
#	if TIKI_ENABLED( TIKI_POINTER_64 )
		//uint64	m_data[ 5u ];
#	elif TIKI_ENABLED( TIKI_POINTER_32 )
		sint32	m_data[ 6u ];
#	endif
#endif
	};

	class MutexLock
	{
	public:

						MutexLock( const Mutex& mutex );
						~MutexLock();

	private:

		const Mutex&	m_mutex;
	};
}