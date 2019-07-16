#pragma once

#include "conct_core.h"

namespace conct
{
	class DynamicMemory;

	namespace memory
	{
		DynamicMemory*	getDynamicMemory();

		void*			allocateSystemMemory( uintptr size, uintptr alignment );
		void			freeSystemMemory( void* pAddress );

		void			beginThreadAllocator( uintptr size );
		void			protectThreadAllocator();
		void			unprotectThreadAllocator();
		void			endThreadAllocator();

		void*			allocateMemory( uintptr size, uintptr alignment = 0u );
		void			freeMemory( void* pAddress );
	}
}

