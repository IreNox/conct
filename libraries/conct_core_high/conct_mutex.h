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

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS)
#	if CONCT_ENABLED( CONCT_POINTER_64 )
		uint64	m_data[ 5u ];
#	elif CONCT_ENABLED( CONCT_POINTER_32 )
		uint32	m_data[ 6u ];
#	endif
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
		sint32	m_data[ 1u ];
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
#	if CONCT_ENABLED( CONCT_POINTER_64 )
		//uint64	m_data[ 5u ];
#	elif CONCT_ENABLED( CONCT_POINTER_32 )
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