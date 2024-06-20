#include "conct_thread_local_storage.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <windows.h>
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
#	include <pthread.h>
#endif

namespace conct
{
	ThreadLocalStorageHandle thread_local_storage::create()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		return TlsAlloc();
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
		pthread_key_t key;
		if( pthread_key_create( &key, nullptr ) != 0u )
		{
			return 0u;
		}
		return key;
#endif
	}

	void thread_local_storage::destroy( ThreadLocalStorageHandle handle )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		TlsFree( handle );
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
		pthread_key_delete( handle );
#endif
	}

	uintptr thread_local_storage::getValue( ThreadLocalStorageHandle handle )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		return (uintptr)TlsGetValue( handle );
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
		return (uintptr)pthread_getspecific( handle );
#endif
	}

	void thread_local_storage::setValue( ThreadLocalStorageHandle handle, uintptr value )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		TlsSetValue( handle, (void*)value );
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
		pthread_setspecific( handle, (void*)value );
#endif
	}
}