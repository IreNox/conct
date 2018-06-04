#include "conct_memory.h"

#if CONCT_ENABLED( CONCT_COMPILER_MSVC )
#	include <memory.h>
#elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
#	include <string.h>
#endif


namespace conct
{
	int	memory::compare( const void* pData1, const void* pData2, uintreg sizeInBytes )
	{
		return memcmp( pData1, pData2, sizeInBytes );
	}

	void memory::copy( void* pTargetData, const void* pSourceData, uintreg sizeInBytes )
	{
		memcpy( pTargetData, pSourceData, sizeInBytes );
	}
}
