#pragma once

#include "conct_core.h"

namespace conct
{
	bool isStringEqual( const char* pString1, const char* pString2 );

	void copyMemory( void* pTarget, const void* pSource, uintreg count );
}