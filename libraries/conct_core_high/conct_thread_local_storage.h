#pragma once

#include "conct_core.h"

namespace conct
{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
	typedef uint32 ThreadLocalStorageHandle;
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
	typedef sint32 ThreadLocalStorageHandle;
#endif

	namespace thread_local_storage
	{
		ThreadLocalStorageHandle		create();
		void							destroy( ThreadLocalStorageHandle handle );

		uintptr							getValue( ThreadLocalStorageHandle handle );
		void							setValue( ThreadLocalStorageHandle handle, uintptr value );
	}
}