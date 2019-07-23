#include "conct_crypto_random.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	pragma comment(lib, "Bcrypt.lib")

#	include <windows.h>
#	include <bcrypt.h>
#endif

namespace conct
{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
	void crypto::calculateRandomBytes( uint8* pBuffer, uintreg bufferSize )
	{
		BCryptGenRandom( NULL, pBuffer, (ULONG)bufferSize, BCRYPT_USE_SYSTEM_PREFERRED_RNG );
	}
#else
#	error "Platform not supported"
#endif
}
