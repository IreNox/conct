#pragma once

#include "conct_core.h"
#include "conct_number_limits.h"

#if CONCT_ENABLED( CONCT_COMPILER_MSVC ) && CONCT_ENABLED( CONCT_REGISTER_64 )
#	include <intrin.h>
#endif

namespace conct
{
	template<typename T>
	CONCT_FORCE_INLINE T min( T a, T b )
	{
		return a < b ? a : b;
	}

	template<typename T>
	CONCT_FORCE_INLINE T max( T a, T b )
	{
		return a > b ? a : b;
	}

	template<typename T>
	CONCT_FORCE_INLINE T* addPointer( T* pPtr, uintreg byteCountToAdd )
	{
		return static_cast< T* >( ( void* )( ( uint8* )pPtr + byteCountToAdd ) );
	}

	template<typename T>
	CONCT_FORCE_INLINE const T* addPointer( const T* pPtr, uintreg byteCountToAdd )
	{
		return static_cast< const T* >( ( const void* )( ( const uint8* )pPtr + byteCountToAdd ) );
	}

	template<typename T>
	CONCT_FORCE_INLINE T* addPointerCast( void* pPtr, uintreg byteCountToAdd )
	{
		return static_cast< T* >( static_cast< void* >( static_cast< uint8* >( pPtr ) + byteCountToAdd ) );
	}

	template<typename T>
	CONCT_FORCE_INLINE const T* addPointerCast( const void* pPtr, uintreg byteCountToAdd )
	{
		return static_cast< const T* >( static_cast< const void* >( static_cast< const uint8* >( pPtr ) + byteCountToAdd ) );
	}

	CONCT_FORCE_INLINE bool isBitSet( uint32 bitMask, uint32 flag )
	{
		return ( bitMask & flag ) == flag;
	}

	CONCT_FORCE_INLINE bool isBitSet64( uint64 bitMask, uint64 flag )
	{
		return ( bitMask & flag ) == flag;
	}

	template<typename T>
	CONCT_FORCE_INLINE bool isPowerOfTwo( T x )
	{
		return ( x & ( x - 1 ) ) == 0;
	}

	template<typename T>
	CONCT_FORCE_INLINE T alignValue( T value, T alignment )
	{
		CONCT_ASSERT( alignment > 0 );
		CONCT_ASSERT( isPowerOfTwo( alignment ) );
		return ( value + alignment - 1 ) & ( 0 - alignment );
	}

	template<typename T>
	CONCT_FORCE_INLINE bool isValueAligned( T value, T alignment )
	{
		return ( value % alignment ) == 0;
	}

	CONCT_FORCE_INLINE bool isPointerAligned( const void* value, uintreg alignment )
	{
		return ( uintptr( value ) % alignment ) == 0;
	}

	template<typename T>
	CONCT_FORCE_INLINE T* alignPointer( T* pPtr, uintreg alignment )
	{
		return ( T* )alignValue< uintreg >( uintptr( pPtr ), alignment );
	}

	template<typename T>
	CONCT_FORCE_INLINE const T* alignPointer( const T* pPtr, uintreg alignment )
	{
		return ( const T* )alignValue< uintreg >( uintptr( pPtr ), alignment );
	}

	CONCT_FORCE_INLINE uintreg countPopulation64( uint64 w )
	{
		uint64 w1 = ( w & 0x2222222222222222 ) + ( ( w + w ) & 0x2222222222222222 );
		uint64 w2 = ( w >> 1 & 0x2222222222222222 ) + ( w >> 2 & 0x2222222222222222 );
		w1 = w1 + ( w1 >> 4 ) & 0x0f0f0f0f0f0f0f0f;
		w2 = w2 + ( w2 >> 4 ) & 0x0f0f0f0f0f0f0f0f;
		return ( w1 + w2 ) * 0x0101010101010101 >> 57;
	}

	CONCT_FORCE_INLINE uintreg countLeadingZeros64( uint64 value )
	{
#if CONCT_ENABLED( CONCT_COMPILER_MSVC ) && CONCT_ENABLED( CONCT_REGISTER_64 )
		unsigned long result = 0u;
		if( _BitScanReverse64( &result, value ) )
		{
			return 63u - result;
		}
		else
		{
			return 64u;
		}
#else
		uint64 x = value;
		x |= ( x >> 1 );
		x |= ( x >> 2 );
		x |= ( x >> 4 );
		x |= ( x >> 8 );
		x |= ( x >> 16 );
		x |= ( x >> 32 );
		return ( 64u - countPopulation64( x ) );
#endif
	}

	template<class T>
	CONCT_FORCE_INLINE T clamp( T value, T min, T max )
	{
		return ( value < min ? min : value > max ? max : value );
	}

	template<typename T>
	CONCT_FORCE_INLINE T getNextPowerOfTwo( T value )
	{
		const T shift = ( T )( 64u - countLeadingZeros64( value - 1 ) );
		return T( 1 ) << shift;
	}

	template<typename TTarget, typename TSource>
	CONCT_FORCE_INLINE bool rangeCheckCast( TTarget& outValue, const TSource& value )
	{
		if( NumberLimits< TSource >::isSigned() && !NumberLimits< TTarget >::isSigned() && value < 0 )
		{
			return false;
		}

		if( value < ( TSource )NumberLimits< TTarget >::lowest() )
		{
			return false;
		}
		else if( value > ( TSource )NumberLimits< TTarget >::max() )
		{
			return false;
		}

		outValue = ( TTarget )value;
		return true;
	}

	template<typename T>
	CONCT_FORCE_INLINE void swap( T& value1, T& value2 )
	{
		T backup = value1;
		value1 = value2;
		value2 = backup;
	}
}
