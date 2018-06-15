#include "conct_trace.h"

#include "conct_dynamic_string.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <windows.h>
#	include <DbgHelp.h>
#endif

#include <stdio.h>

namespace conct
{
	void trace::write( const char* pString )
	{
		puts( pString );
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		OutputDebugStringA( pString );
#endif
	}

	void trace::write( const DynamicString& string )
	{
		write( string.toConstCharPointer() );
	}
}
