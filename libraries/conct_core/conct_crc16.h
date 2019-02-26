#pragma once

#include "conct_core.h"

namespace conct
{
	uint16	calculateCrc16( const void* pData, uintreg size );
	uint16	calculateStringCrc16( const char* pString );
}