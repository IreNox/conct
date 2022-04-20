#include "conct_crypto_random.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	pragma comment(lib, "Bcrypt.lib")

#	include <windows.h>
#	include <bcrypt.h>
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
#	include "conct_trace.h"

#	include <fcntl.h>
#	include <sys/types.h>
#	include <unistd.h>
#elif CONCT_ENABLED( CONCT_PLATFORM_ESP )
#	include <sodium/randombytes.h>
#endif

namespace conct
{
	void crypto::calculateRandomBytes( uint8* pBuffer, uintreg bufferSize )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		BCryptGenRandom( NULL, pBuffer, (ULONG)bufferSize, BCRYPT_USE_SYSTEM_PREFERRED_RNG );
#elif CONCT_ENABLED( CONCT_PLATFORM_POSIX )
		int file = open( "/dev/urandom", O_RDONLY );
		if( file >= 0 )
		{
			read( file, pBuffer, bufferSize );
			close( file );
		}
		else
		{
			trace::write( "critical: failed to get secure random bytes." );
		}
#elif CONCT_ENABLED( CONCT_PLATFORM_ESP )
	randombytes_buf( pBuffer, bufferSize );
#else
#	error "Platform not supported"
#endif
	}
}
