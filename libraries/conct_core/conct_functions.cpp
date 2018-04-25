#include "conct_functions.h"

#include <string.h>

namespace conct
{
	bool isStringEqual( const char* pString1, const char* pString2 )
	{
		return strcmp( pString1, pString2 ) == 0;
	}

	void copyMemory( void* pTarget, const void* pSource, uintreg count )
	{
		memcpy( pTarget, pSource, count );
	}
}
