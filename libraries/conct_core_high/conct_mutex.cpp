#include "conct_mutex.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS)
#	include <windows.h>
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
#	include "conct_trace.h"

#	include <pthread.h>
#endif

namespace conct
{
	Mutex::Mutex()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		CONCT_STATIC_ASSERT( sizeof( m_data ) == sizeof( CRITICAL_SECTION ) );
		CONCT_STATIC_ASSERT( CONCT_ALIGNOF( m_data ) == CONCT_ALIGNOF( CRITICAL_SECTION ) );

		InitializeCriticalSection( ( CRITICAL_SECTION* )m_data );
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
		CONCT_STATIC_ASSERT( sizeof( m_data ) == sizeof( pthread_mutex_t ) );
		CONCT_STATIC_ASSERT( CONCT_ALIGNOF( m_data ) == CONCT_ALIGNOF( pthread_mutex_t ) );

		if( pthread_mutex_init( ( pthread_mutex_t* )m_data, nullptr ) < 0 )
		{
			trace::write( "critical: pthread_mutex_init failed." );
		}
#endif
	}

	Mutex::~Mutex()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS)
		DeleteCriticalSection( ( CRITICAL_SECTION* )m_data );
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
		pthread_mutex_destroy( ( pthread_mutex_t* )m_data );
#endif
	}

	void Mutex::lock() const
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS)
		EnterCriticalSection( ( CRITICAL_SECTION* )m_data );
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
		pthread_mutex_lock( ( pthread_mutex_t* )m_data );
#endif
	}

	bool Mutex::tryLock() const
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS)
		return TryEnterCriticalSection( ( CRITICAL_SECTION* )m_data ) ? true : false;
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
		return pthread_mutex_trylock( ( pthread_mutex_t* )m_data ) == 0;
#endif
	}

	void Mutex::unlock() const
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS)
		LeaveCriticalSection( ( CRITICAL_SECTION* )m_data );
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
		pthread_mutex_unlock( ( pthread_mutex_t* )m_data );
#endif
	}

	MutexLock::MutexLock( const Mutex& mutex )
		: m_mutex( mutex )
	{
		m_mutex.lock();
	}

	MutexLock::~MutexLock()
	{
		m_mutex.unlock();
	}
}