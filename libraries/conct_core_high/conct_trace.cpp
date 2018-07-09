#include "conct_trace.h"

#include "conct_dynamic_string.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <windows.h>
#	include <DbgHelp.h>
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
#	include <android/log.h>
#endif

#include <stdio.h>

namespace conct
{
	void trace::write( const char* pString )
	{
		puts( pString );
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		OutputDebugStringA( pString );
		OutputDebugStringA( "\n" );
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
		__android_log_write( ANDROID_LOG_INFO, "conct", pString );
#endif
	}

	void trace::write( const DynamicString& string )
	{
		write( string.toConstCharPointer() );
	}
}
