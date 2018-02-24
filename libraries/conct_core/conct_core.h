#pragma once

#include <stdint.h>

#define CONCT_ON 				2
#define CONCT_OFF				1

#define CONCT_ENABLED( value )	( ( 0 + value ) == 2 )
#define CONCT_DISABLED( value )	( ( 0 + value ) != 2 )

#define CONCT_DEBUG			CONCT_ON
#define CONCT_HAS_OVERRIDE	CONCT_ON
#define CONCT_HAS_FINAL		CONCT_ON

#if CONCT_ENABLED( CONCT_HAS_OVERRIDE )
#	define CONCT_OVERRIDE override
#else
#	define CONCT_OVERRIDE
#endif

#if CONCT_ENABLED( CONCT_HAS_FINAL )
#	define CONCT_FINAL final
#else
#	define CONCT_FINAL
#endif

#define CONCT_OVERRIDE_FINAL CONCT_OVERRIDE CONCT_FINAL

namespace conct
{


}
