#include "conct_thread_local_storage.h"

#if TIKI_ENABLED( TIKI_PLATFORM_WINDOWS )
#	include <windows.h>
#elif TIKI_ENABLED( TIKI_PLATFORM_POSIX )
#	include <pthread.h>
#endif

namespace conct
{
	ThreadLocalStorageHandle thread_local_storage::create()
	{
#if TIKI_ENABLED( TIKI_PLATFORM_WINDOWS )
		return TlsAlloc();
#elif TIKI_ENABLED( TIKI_PLATFORM_POSIX )
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
#if TIKI_ENABLED( TIKI_PLATFORM_WINDOWS )
		TlsFree( handle );
#elif TIKI_ENABLED( TIKI_PLATFORM_POSIX )
		pthread_key_delete( handle );
#endif
	}

	uintptr thread_local_storage::getValue( ThreadLocalStorageHandle handle )
	{
#if TIKI_ENABLED( TIKI_PLATFORM_WINDOWS )
		return (uintptr)TlsGetValue( handle );
#elif TIKI_ENABLED( TIKI_PLATFORM_POSIX )
		return (uintptr)pthread_getspecific( handle );
#endif
	}

	void thread_local_storage::setValue( ThreadLocalStorageHandle handle, uintptr value )
	{
#if TIKI_ENABLED( TIKI_PLATFORM_WINDOWS )
		TlsSetValue( handle, (void*)value );
#elif TIKI_ENABLED( TIKI_PLATFORM_POSIX )
		pthread_setspecific( handle, (void*)value );
#endif
	}
}