#pragma once

#include "conct_core.h"

namespace conct
{
	class DynamicMemory;

	namespace memory
	{
		DynamicMemory*	getDynamicMemory();

		void*			allocateSystemMemory( uintptr size, uintptr alignment, size_t address );
		void			freeSystemMemory( void* pAddress );

		void			protectDynamicMemory();
		void			unprotectDynamicMemory();

		void			beginThreadAllocator( uintptr size );
		void			endThreadAllocator();
		void			endAndProtectThreadAllocator();

		void*			allocateMemory( uintptr size, uintptr alignment = 0u );
		void			freeMemory( void* pAddress );
	}

	class DynamicMemoryUnprotectionScope
	{
	public:

		DynamicMemoryUnprotectionScope()
		{
			memory::unprotectDynamicMemory();
		}

		~DynamicMemoryUnprotectionScope()
		{
			memory::protectDynamicMemory();
		}
	};
}

