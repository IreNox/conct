#pragma once

#include "conct_platform.h"

#include <stdint.h>

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

	CONCT_STATIC_ASSERT( sizeof( uint8 ) == 1u );
	CONCT_STATIC_ASSERT( sizeof( sint8 ) == 1u );
	CONCT_STATIC_ASSERT( sizeof( uint16 ) == 2u );
	CONCT_STATIC_ASSERT( sizeof( sint16 ) == 2u );
	CONCT_STATIC_ASSERT( sizeof( uint32 ) == 4u );
	CONCT_STATIC_ASSERT( sizeof( sint32 ) == 4u );
	CONCT_STATIC_ASSERT( sizeof( uint64 ) == 8u );
	CONCT_STATIC_ASSERT( sizeof( sint64 ) == 8u );

	typedef int			sint;
	typedef unsigned	uint;

#if CONCT_ENABLED( CONCT_POINTER_16 )
	typedef uint16		uintptr;
	CONCT_STATIC_ASSERT( sizeof( void* ) == 2u );
#elif CONCT_ENABLED( CONCT_POINTER_32 )
	typedef uint32		uintptr;
	CONCT_STATIC_ASSERT( sizeof( void* ) == 4u );
#elif CONCT_ENABLED( CONCT_POINTER_64 )
	typedef uint64		uintptr;
	CONCT_STATIC_ASSERT( sizeof( void* ) == 8u );
#else
#	error "No Pointer size"
#endif

#if CONCT_ENABLED( CONCT_REGISTER_8 )
	typedef uint8		uintreg;
#elif CONCT_ENABLED( CONCT_REGISTER_16 )
	typedef uint16		uintreg;
#elif CONCT_ENABLED( CONCT_REGISTER_32 )
	typedef uint32		uintreg;
#elif CONCT_ENABLED( CONCT_REGISTER_64 )
	typedef uint64		uintreg;
#else
#	error "No Register size"
#endif

	typedef uint8		DeviceId;
	typedef uint16		TypeCrc;
	typedef uint16		InstanceId;
	typedef uint16		PercentValue;

	struct Instance
	{
		InstanceId	id;
		TypeCrc		type;
	};

	struct Decimal
	{
		sint32		number;
	};

	static const DeviceId InvalidDeviceId = 0u;
	static const DeviceId FirstDeviceId = 1u;
	static const TypeCrc InvalidTypeCrc = 0u;
	static const InstanceId InvalidInstanceId = 0xffffu;
	static const Instance InvalidInstance = { InvalidInstanceId, InvalidTypeCrc };
	static const PercentValue PercentValueFull = 0xffffu;
}
