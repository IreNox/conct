#pragma once

#include <cctype>
#include <vector>
#include <string>
#include <filesystem>

namespace std
{
	namespace filesystem = std::experimental::filesystem;
}

namespace conct
{
	typedef uint16_t TypeCrc;

	std::string getExecutableName();
	uint16_t	calculateCrc16( const void* pData, size_t size );
}