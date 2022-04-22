#pragma once

#include "conct_core.h"

namespace conct
{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
	typedef uint32 ThreadLocalStorageHandle;
	static const ThreadLocalStorageHandle InvalidThreadLocalStorageHandle = 0xffffffffu;
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
	typedef unsigned int ThreadLocalStorageHandle;
	static const ThreadLocalStorageHandle InvalidThreadLocalStorageHandle = 0u;
#endif

	namespace thread_local_storage
	{
		ThreadLocalStorageHandle		create();
		void							destroy( ThreadLocalStorageHandle handle );

		uintptr							getValue( ThreadLocalStorageHandle handle );
		void							setValue( ThreadLocalStorageHandle handle, uintptr value );
	}
}