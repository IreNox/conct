#pragma once

#include <stdint.h>

#define CONCT_ON 				2
#define CONCT_OFF				1

#define CONCT_ENABLED( value )	( ( 0 + value ) == 2 )
#define CONCT_DISABLED( value )	( ( 0 + value ) != 2 )
#define CONCT_IF( expr )		( ( expr ) ? CONCT_ON : CONCT_OFF )

#if defined( _WIN32 ) // Windows
#	define CONCT_WINDOWS			CONCT_ON
#	if defined( _WIN64 )
#		define CONCT_POINTER_64		CONCT_ON
#		define CONCT_REGISTER_64	CONCT_ON
#	else
#		define CONCT_POINTER_32		CONCT_ON
#		define CONCT_REGISTER_32	CONCT_ON
#	endif
#else
#	define CONCT_WINDOWS			CONCT_OFF
#endif

#if defined( __AVR__ ) // AVR (UNO, Duemilanove, etc.)
#	define CONCT_AVR				CONCT_ON
#	define CONCT_POINTER_16			CONCT_ON
#	define CONCT_REGISTER_8			CONCT_ON
#else
#	define CONCT_AVR				CONCT_OFF
#endif

#if defined( __SAM3X8E__ ) // ARM (DUE)
#	define CONCT_ARM_DUE			CONCT_ON
#	define CONCT_POINTER_32			CONCT_ON
#	define CONCT_REGISTER_32		CONCT_ON
#else
#	define CONCT_ARM_DUE			CONCT_OFF
#endif

#if !defined( CONCT_POINTER_16 )
#	define CONCT_POINTER_16				CONCT_OFF
#endif
#if !defined( CONCT_POINTER_32 )
#	define CONCT_POINTER_32				CONCT_OFF
#endif
#if !defined( CONCT_POINTER_64 )
#	define CONCT_POINTER_64				CONCT_OFF
#endif

#if !defined( CONCT_REGISTER_8 )
#	define CONCT_REGISTER_8				CONCT_OFF
#endif
#if !defined( CONCT_REGISTER_16 )
#	define CONCT_REGISTER_16			CONCT_OFF
#endif
#if !defined( CONCT_REGISTER_32 )
#	define CONCT_REGISTER_32			CONCT_OFF
#endif
#if !defined( CONCT_REGISTER_64 )
#	define CONCT_REGISTER_64			CONCT_OFF
#endif

#define CONCT_DEBUG					CONCT_ON
#define CONCT_HAS_OVERRIDE			CONCT_ON
#define CONCT_HAS_FINAL				CONCT_ON
#define CONCT_HAS_BREAK				CONCT_WINDOWS

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

#if CONCT_ENABLED( CONCT_HAS_BREAK )
#	define CONCT_BREAK __debugbreak()
#endif

#if CONCT_ENABLED( CONCT_DEBUG ) && CONCT_ENABLED( CONCT_HAS_BREAK )
#	define CONCT_ASSERT( expr ) if( !( expr ) ) CONCT_BREAK
#else
#	define CONCT_ASSERT( expr )
#endif

#define CONCT_COUNT( arr ) ( sizeof( arr ) / sizeof( *arr ) )

#define CONCT_MIN( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define CONCT_MAX( a, b ) ( ( a ) > ( b ) ? ( a ) : ( b ) )

namespace conct
{
	class Proxy;

	typedef uint8_t		uint8;
	typedef int8_t		sint8;
	typedef uint16_t	uint16;
	typedef int16_t		sint16;
	typedef uint32_t	uint32;
	typedef int32_t		sint32;
	typedef uint64_t	uint64;
	typedef int64_t		sint64;

	typedef uint8		muint;

	typedef int			sint;
	typedef unsigned	uint;

#if CONCT_ENABLED( CONCT_POINTER_16 )
	typedef uint16		puint;
#elif CONCT_ENABLED( CONCT_POINTER_32 )
	typedef uint32		puint;
#elif CONCT_ENABLED( CONCT_POINTER_64 )
	typedef uint64		puint;
#endif

	typedef uint8		DeviceId;
	typedef uint16		TypeCrc;
	typedef uint16		InstanceId;
	typedef uint16		PercentValue;

	struct LocalInstance
	{
		InstanceId	id;
		void*		pInstance;
		Proxy*		pProxy;
	};

	bool	isStringEqual( const char* pString1, const char* pString2 );
	void	copyMemory( void* pTarget, const void* pSource, muint size );
}
