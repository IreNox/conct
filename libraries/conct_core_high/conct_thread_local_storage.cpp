#include "conct_thread_local_storage.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <windows.h>
#endif

namespace conct
{
	ThreadLocalStorageHandle thread_local_storage::create()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		return TlsAlloc();
#endif
	}

	void thread_local_storage::destroy( ThreadLocalStorageHandle handle )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		TlsFree( handle );
#endif
	}

	uintptr thread_local_storage::getValue( ThreadLocalStorageHandle handle )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		return (uintptr)TlsGetValue( handle );
#endif
	}

	void thread_local_storage::setValue( ThreadLocalStorageHandle handle, uintptr value )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		TlsSetValue( handle, (void*)value );
#endif
	}
}