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
	std::string getExecutableName();
}