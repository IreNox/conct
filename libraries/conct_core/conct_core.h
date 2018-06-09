#pragma once

#include <stdint.h>

#define CONCT_ON 				2
#define CONCT_OFF				1

#define CONCT_ENABLED( value )	( ( 0 + value ) == 2 )
#define CONCT_DISABLED( value )	( ( 0 + value ) != 2 )
#define CONCT_IF( expr )		( ( expr ) ? CONCT_ON : CONCT_OFF )

#if defined( _WIN32 ) // Windows
#	define CONCT_PLATFORM_WINDOWS	CONCT_ON
#	if defined( _WIN64 )
#		define CONCT_POINTER_64		CONCT_ON
#		define CONCT_REGISTER_64	CONCT_ON
#	else
#		define CONCT_POINTER_32		CONCT_ON
#		define CONCT_REGISTER_32	CONCT_ON
#	endif
#else
#	define CONCT_PLATFORM_WINDOWS	CONCT_OFF
#endif

#if defined( __AVR__ ) // AVR (UNO, Duemilanove, etc.)
#	define CONCT_PLATFORM_AVR		CONCT_ON
#	define CONCT_POINTER_16			CONCT_ON
#	define CONCT_REGISTER_8			CONCT_ON
#else
#	define CONCT_PLATFORM_AVR				CONCT_OFF
#endif

#if defined( __SAM3X8E__ ) // ARM (DUE)
#	define CONCT_PLATFORM_ARM_DUE	CONCT_ON
#	define CONCT_POINTER_32			CONCT_ON
#	define CONCT_REGISTER_32		CONCT_ON
#else
#	define CONCT_PLATFORM_ARM_DUE	CONCT_OFF
#endif

#if defined( _MSC_VER )
#	define CONCT_COMPILER_MSVC		CONCT_ON
#else
#	define CONCT_COMPILER_MSVC		CONCT_OFF
#endif

#if defined( __clang__ )
#	define CONCT_COMPILER_CLANG		CONCT_ON
#else
#	define CONCT_COMPILER_CLANG		CONCT_OFF
#endif

#if defined( __GNUC__ )
#	define CONCT_COMPILER_GCC		CONCT_ON
#else
#	define CONCT_COMPILER_GCC		CONCT_OFF
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
#define CONCT_HAS_BREAK				CONCT_PLATFORM_WINDOWS

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

#if CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
#	define CONCT_OFFSETOF( type, member )		( __builtin_offsetof( type, member ) )
#else
#	define CONCT_OFFSETOF( type, member )		( (uint)(&((type*)nullptr)->member) )
#endif


#define CONCT_MIN( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define CONCT_MAX( a, b ) ( ( a ) > ( b ) ? ( a ) : ( b ) )

#define CONCT_USE_INLINE CONCT_ON

#if CONCT_ENABLED( CONCT_USE_INLINE )

#	if CONCT_ENABLED( CONCT_COMPILER_MSVC )
#		define CONCT_INLINE			inline
#		define CONCT_FORCE_INLINE	__forceinline
#		define CONCT_NO_INLINE		__declspec(noinline)
#	elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_GCC )
#		define CONCT_INLINE			inline
#		define CONCT_FORCE_INLINE	__attribute__((always_inline))
#		define CONCT_NO_INLINE		__attribute__((noinline))
#	else
#		error Platform not implemented
#	endif
#else
#	define CONCT_INLINE
#	define CONCT_FORCE_INLINE
#	define CONCT_NO_INLINE
#endif

#define CONCT_STATIC_ASSERT( expr ) static_assert( ( expr ), #expr )

namespace conct
{
	typedef uint8_t		uint8;
	typedef int8_t		sint8;
	typedef uint16_t	uint16;
	typedef int16_t		sint16;
	typedef uint32_t	uint32;
	typedef int32_t		sint32;
	typedef uint64_t	uint64;
	typedef int64_t		sint64;

	typedef int			sint;
	typedef unsigned	uint;

#if CONCT_ENABLED( CONCT_POINTER_16 )
	typedef uint16		uintptr;
#elif CONCT_ENABLED( CONCT_POINTER_32 )
	typedef uint32		uintptr;
#elif CONCT_ENABLED( CONCT_POINTER_64 )
	typedef uint64		uintptr;
#endif

#if CONCT_ENABLED( CONCT_REGISTER_8 )
	typedef uint8		uintreg;
#endif
#if CONCT_ENABLED( CONCT_REGISTER_16 )
	typedef uint16		uintreg;
#endif
#if CONCT_ENABLED( CONCT_REGISTER_32 )
	typedef uint32		uintreg;
#endif
#if CONCT_ENABLED( CONCT_REGISTER_64 )
	typedef uint64		uintreg;
#endif

	typedef uint8		DeviceId;
	typedef uint16		TypeCrc;
	typedef uint16		InstanceId;
	typedef uint16		PercentValue;
	typedef uint32		Guid;

	static const DeviceId InvalidDeviceId = 0u;
	static const InstanceId InvalidInstanceId = 0xffffu;
}
