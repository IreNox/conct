#include "base.h"

#if defined( _WIN32 )
#	include <windows.h>
#endif

namespace conct
{
	std::string getExecutableName()
	{
#if defined( _WIN32 )
		char exeFileName[ MAX_PATH ];
		GetModuleFileNameA( nullptr, exeFileName, sizeof( exeFileName ) );

		return exeFileName;
#else
#	error "Platform not supported"
#endif
	}
}