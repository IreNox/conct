#pragma once

#define CONCT_ON 				2
#define CONCT_OFF				1

#define CONCT_ENABLED( value )	( ( 0 + value ) == 2 )
#define CONCT_DISABLED( value )	( ( 0 + value ) != 2 )

#define CONCT_IF( expr )		( ( expr ) ? CONCT_ON : CONCT_OFF )

#if defined( _WIN32 ) // Windows
#	define CONCT_PLATFORM_WINDOWS		CONCT_ON
#	if defined( _WIN64 )
#		define CONCT_POINTER_64			CONCT_ON
#		define CONCT_REGISTER_64		CONCT_ON
#	else
#		define CONCT_POINTER_32			CONCT_ON
#		define CONCT_REGISTER_32		CONCT_ON
#	endif
#	if defined( _WINDLL )
#		define CONCT_TARGET_DLL			CONCT_ON
#	endif
#	define CONCT_HAS_BREAK				CONCT_ON
#else
#	define CONCT_PLATFORM_WINDOWS		CONCT_OFF
#endif

#if defined( __ANDROID__ )
#	define CONCT_PLATFORM_ANDROID		CONCT_ON
#	define CONCT_PLATFORM_POSIX			CONCT_ON
#	define CONCT_HAS_BREAK				CONCT_ON
#else
#	define CONCT_PLATFORM_ANDROID		CONCT_OFF
#endif

#if defined( __linux__ ) && !defined( __ANDROID__ )
#	define CONCT_PLATFORM_LINUX			CONCT_ON
#	define CONCT_PLATFORM_POSIX			CONCT_ON
#	define CONCT_HAS_BREAK				CONCT_ON
#else
#	define CONCT_PLATFORM_LINUX			CONCT_OFF
#endif

#if defined( __AVR__ ) // AVR (UNO, Duemilanove, etc.)
#	define CONCT_PLATFORM_AVR			CONCT_ON
#	define CONCT_PLATFORM_ARDUINO		CONCT_ON
#	define CONCT_POINTER_16				CONCT_ON
#	define CONCT_REGISTER_8				CONCT_ON
#else
#	define CONCT_PLATFORM_AVR			CONCT_OFF
#endif

#if defined( __SAM3X8E__ ) // ARM (DUE)
#	define CONCT_PLATFORM_ARM_DUE		CONCT_ON
#	define CONCT_PLATFORM_ARDUINO		CONCT_ON
#	define CONCT_POINTER_32				CONCT_ON
#	define CONCT_REGISTER_32			CONCT_ON
#else
#	define CONCT_PLATFORM_ARM_DUE		CONCT_OFF
#endif

#if defined( ESP_PLATFORM ) // ESP
#	define CONCT_PLATFORM_ESP			CONCT_ON
#	define CONCT_POINTER_32				CONCT_ON
#	define CONCT_REGISTER_32			CONCT_ON
#else
#	define CONCT_PLATFORM_ESP			CONCT_OFF
#endif

#if !defined( CONCT_ENVIRONMENT_SIMULATOR )
#	define CONCT_ENVIRONMENT_SIMULATOR	CONCT_OFF
#endif

#if !defined( CONCT_ENVIRONMENT_TESTS )
#	define CONCT_ENVIRONMENT_TESTS		CONCT_OFF
#endif

#if defined( _MSC_VER )
#	define CONCT_COMPILER_MSVC			CONCT_ON
#else
#	define CONCT_COMPILER_MSVC			CONCT_OFF
#endif

#if defined( __clang__ )
#	define CONCT_COMPILER_CLANG			CONCT_ON
#else
#	define CONCT_COMPILER_CLANG			CONCT_OFF
#endif

#if defined( __GNUC__ )
#	define CONCT_COMPILER_GCC			CONCT_ON
#else
#	define CONCT_COMPILER_GCC			CONCT_OFF
#endif

#if CONCT_ENABLED( CONCT_COMPILER_MSVC )
#	if defined( _M_IX86 )
#		define CONCT_ARCH_X86			CONCT_ON
#	elif defined( _M_AMD64 )
#		define CONCT_ARCH_X64			CONCT_ON
#	elif defined( _M_ARM )
#		define CONCT_ARCH_ARM32			CONCT_ON
#	elif defined( _M_ARM64 )
#		define CONCT_ARCH_ARM64			CONCT_ON
#	endif
#endif

#if CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
#	if defined( __aarch64__ ) || defined( __amd64__ )
#		define CONCT_POINTER_64			CONCT_ON
#		define CONCT_REGISTER_64		CONCT_ON
#	elif defined( __arm__ ) || defined( __i386__ )
#		define CONCT_POINTER_32			CONCT_ON
#		define CONCT_REGISTER_32		CONCT_ON
#	endif
#	if defined( __i386__ )
#		define CONCT_ARCH_X86			CONCT_ON
#	elif defined( __amd64__ )
#		define CONCT_ARCH_X64			CONCT_ON
#	elif defined( __arm__ )
#		define CONCT_ARCH_ARM32			CONCT_ON
#	elif defined( __aarch64__ )
#		define CONCT_ARCH_ARM64			CONCT_ON
#	endif
#endif

#if !defined( CONCT_ARCH_X86 )
#	define CONCT_ARCH_X86				CONCT_OFF
#endif

#if !defined( CONCT_ARCH_X64 )
#	define CONCT_ARCH_X64				CONCT_OFF
#endif

#if !defined( CONCT_ARCH_ARM32 )
#	define CONCT_ARCH_ARM32				CONCT_OFF
#endif

#if !defined( CONCT_ARCH_ARM64 )
#	define CONCT_ARCH_ARM64				CONCT_OFF
#endif

#if !defined( CONCT_PLATFORM_ARDUINO )
#	define CONCT_PLATFORM_ARDUINO		CONCT_OFF
#endif

#if !defined( CONCT_PLATFORM_POSIX )
#	define CONCT_PLATFORM_POSIX			CONCT_OFF
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

#if !defined( CONCT_HAS_BREAK )
#	define CONCT_HAS_BREAK				CONCT_OFF
#endif

#if !defined( CONCT_TARGET_DLL )
#	define CONCT_TARGET_DLL				CONCT_OFF
#endif

#define CONCT_DEBUG						CONCT_ON
#define CONCT_HAS_OVERRIDE				CONCT_ON
#define CONCT_HAS_FINAL					CONCT_ON
#define CONCT_USE_INLINE				CONCT_ON

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
#	if CONCT_ENABLED( CONCT_COMPILER_MSVC )
#		define CONCT_BREAK __debugbreak()
#	elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
#		define CONCT_BREAK __builtin_trap()
#	endif
#else
#	define CONCT_BREAK
#endif

#if CONCT_ENABLED( CONCT_DEBUG ) && CONCT_ENABLED( CONCT_HAS_BREAK )
#	define CONCT_ASSERT( expr ) if( !( expr ) ) CONCT_BREAK
#else
#	define CONCT_ASSERT( expr )
#endif

#define CONCT_COUNT( arr ) ( sizeof( arr ) / sizeof( *arr ) )

#if CONCT_ENABLED( CONCT_COMPILER_MSVC )
#	define CONCT_ALIGN_PREFIX( var )			__declspec( align( var ) )
#	define CONCT_ALIGN_POSTFIX( var )
#	define CONCT_ALIGNOF( type )				( __alignof( type ) )
#elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
#	define CONCT_ALIGN_PREFIX( var )
#	define CONCT_ALIGN_POSTFIX( var )			__attribute__( ( aligned( var ) ) )
#	define CONCT_ALIGNOF( type )				( __alignof__( type ) )
#endif

#if CONCT_ENABLED( CONCT_ARCH_X86 ) || CONCT_ENABLED( CONCT_ARCH_X64 ) // also for AMD64?
#	if CONCT_ENABLED( CONCT_COMPILER_MSVC )
#		define CONCT_DLL							__declspec(dllexport)
#		define CONCT_CDECL							__stdcall
#	elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
#		define CONCT_DLL							extern
#		define CONCT_CDECL							__attribute__((__cdecl__))
#	endif
#else
#	define CONCT_DLL
#	define CONCT_CDECL
#endif

#if CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
#	define CONCT_OFFSETOF( type, member )		( __builtin_offsetof( type, member ) )
#else
#	define CONCT_OFFSETOF( type, member )		( (uint)(&((type*)nullptr)->member) )
#endif

#define CONCT_MIN( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define CONCT_MAX( a, b ) ( ( a ) > ( b ) ? ( a ) : ( b ) )

#if CONCT_ENABLED( CONCT_USE_INLINE )

#	if CONCT_ENABLED( CONCT_COMPILER_MSVC )
#		define CONCT_INLINE			inline
#		define CONCT_FORCE_INLINE	__forceinline
#		define CONCT_NO_INLINE		__declspec(noinline)
#	elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
#		define CONCT_INLINE			inline
#		define CONCT_FORCE_INLINE	inline __attribute__((always_inline))
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

#define CONCT_USE( var ) (void)var
