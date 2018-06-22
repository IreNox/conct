#include "conct_mutex.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS)
#	include <windows.h>
#endif

namespace conct
{
	Mutex::Mutex()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		CONCT_STATIC_ASSERT( sizeof( m_data ) == sizeof( CRITICAL_SECTION ) );
		CONCT_STATIC_ASSERT( CONCT_ALIGNOF( m_data ) == CONCT_ALIGNOF( CRITICAL_SECTION ) );

		InitializeCriticalSection( ( CRITICAL_SECTION* )m_data );
#endif
	}

	Mutex::~Mutex()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS)
		DeleteCriticalSection( ( CRITICAL_SECTION* )m_data );
#endif
	}

	void Mutex::lock() const
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS)
		EnterCriticalSection( ( CRITICAL_SECTION* )m_data );
#endif
	}

	bool Mutex::tryLock() const
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS)
		return TryEnterCriticalSection( ( CRITICAL_SECTION* )m_data ) ? true : false;
#endif
	}

	void Mutex::unlock() const
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS)
		LeaveCriticalSection( ( CRITICAL_SECTION* )m_data );
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