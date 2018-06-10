#pragma once

#include <cctype>
#include <vector>
#include <string>

namespace conct
{
	typedef uint16_t TypeCrc;

	uint16_t	calculateCrc16( const void* pData, size_t size );
}