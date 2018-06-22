#pragma once

#include "conct_core.h"

namespace conct
{
	typedef uint32 ThreadLocalStorageHandle;

	namespace thread_local_storage
	{
		ThreadLocalStorageHandle		create();
		void							destroy( ThreadLocalStorageHandle handle );

		uintptr							getValue( ThreadLocalStorageHandle handle );
		void							setValue( ThreadLocalStorageHandle handle, uintptr value );
	}
}