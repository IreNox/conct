#include "conct_string_tools.h"

#include "conct_functions.h"
#include "conct_string.h"

#include <stdio.h>
#include <cstdlib>
#include <math.h>

namespace conct
{
	DynamicString string_tools::toString( sint8 value )
	{
		char buffer[ 64u ];
		sprintf( buffer, "%i", value );
		return DynamicString( buffer );
	}

	DynamicString string_tools::toString( sint16 value )
	{
		char buffer[ 64u ];
		sprintf( buffer, "%i", value );
		return DynamicString( buffer );
	}

	DynamicString string_tools::toString( sint32 value )
	{
		char buffer[ 64u ];
		sprintf( buffer, "%i", value );
		return DynamicString( buffer );
	}

	DynamicString string_tools::toString( sint64 value )
	{
		char buffer[ 64u ];
#	if CONCT_ENABLED( CONCT_POINTER_64 ) && CONCT_ENABLED( CONCT_PLATFORM_POSIX )
		sprintf( buffer, "%li", value );
#	elif CONCT_ENABLED( CONCT_POINTER_32 )
		sprintf( buffer, "%lli", value );
#	endif
		return DynamicString( buffer );
	}

	DynamicString string_tools::toString( uint8 value )
	{
		char buffer[ 64u ];
		sprintf( buffer, "%u", value );
		return DynamicString( buffer );
	}

	DynamicString string_tools::toString( uint16 value )
	{
		char buffer[ 64u ];
		sprintf( buffer, "%u", value );
		return DynamicString( buffer );
	}

	DynamicString string_tools::toString( uint32 value )
	{
		char buffer[ 64u ];
		sprintf( buffer, "%u", value );
		return DynamicString( buffer );
	}

	DynamicString string_tools::toString( uint64 value )
	{
		char buffer[ 64u ];
#if CONCT_ENABLED( CONCT_POINTER_32 ) || CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		sprintf( buffer, "%llu", value );
#elif CONCT_ENABLED( CONCT_POINTER_64 )
		sprintf( buffer, "%lu", value );
#endif
		return DynamicString( buffer );
	}

	DynamicString string_tools::toString( float value )
	{
		char buffer[ 64u ];
		sprintf( buffer, "%f", value );

		uintreg index = getStringLength( buffer ) - 1u;
		while( index > 0 )
		{
			if( buffer[ index - 1u ] == '.' || buffer[ index ] != '0' )
			{
				break;
			}
			else if( buffer[ index ] == '0' )
			{
				buffer[ index ] = '\0';
			}

			index--;
		}

		return DynamicString( buffer );
	}

	DynamicString string_tools::toString( double value )
	{
		char buffer[ 64u ];
		sprintf( buffer, "%f", value );

		uintreg index = getStringLength( buffer ) - 1u;
		while( index > 0 )
		{
			if( buffer[ index - 1u ] == '.' || buffer[ index ] != '0' )
			{
				break;
			}
			else if( buffer[ index ] == '0' )
			{
				buffer[ index ] = '\0';
			}

			index--;
		}

		return DynamicString( buffer );
	}

	DynamicString string_tools::toHexString( sint8 value )
	{
		char buffer[ 64u ];
		sprintf( buffer, "%02x", value );
		return DynamicString( buffer );
	}

	DynamicString string_tools::toHexString( sint16 value )
	{
		char buffer[ 64u ];
		sprintf( buffer, "%04x", value );
		return DynamicString( buffer );
	}

	DynamicString string_tools::toHexString( sint32 value )
	{
		char buffer[ 64u ];
		sprintf( buffer, "%08x", value );
		return DynamicString( buffer );
	}

	DynamicString string_tools::toHexString( sint64 value )
	{
		char buffer[ 64u ];
#	if CONCT_ENABLED( CONCT_POINTER_64 ) && CONCT_ENABLED( CONCT_PLATFORM_POSIX )
		sprintf( buffer, "%016lx", value );
#	elif CONCT_ENABLED( CONCT_POINTER_32 )
		sprintf( buffer, "%016llx", value );
#	endif
		return DynamicString( buffer );
	}

	DynamicString string_tools::toHexString( uint8 value )
	{
		char buffer[ 64u ];
		sprintf( buffer, "%02x", value );
		return DynamicString( buffer );
	}

	DynamicString string_tools::toHexString( uint16 value )
	{
		char buffer[ 64u ];
		sprintf( buffer, "%04x", value );
		return DynamicString( buffer );
	}

	DynamicString string_tools::toHexString( uint32 value )
	{
		char buffer[ 64u ];
		sprintf( buffer, "%08x", value );
		return DynamicString( buffer );
	}

	DynamicString string_tools::toHexString( uint64 value )
	{
		char buffer[ 64u ];
#	if CONCT_ENABLED( CONCT_POINTER_64 ) && CONCT_ENABLED( CONCT_PLATFORM_POSIX )
		sprintf( buffer, "%016lx", value );
#	elif CONCT_ENABLED( CONCT_POINTER_32 )
		sprintf( buffer, "%016llx", value );
#	endif
		return DynamicString( buffer );
	}

	DynamicString string_tools::toHexString( float value )
	{
		char buffer[ 64u ];
		sprintf( buffer, "%08x", *(uint32*)&value );
		return DynamicString( buffer );
	}

	DynamicString string_tools::toHexString( double value )
	{
		char buffer[ 64u ];
#	if CONCT_ENABLED( CONCT_POINTER_64 ) && CONCT_ENABLED( CONCT_PLATFORM_POSIX )
		sprintf( buffer, "%016lx", *(uint64*)&value );
#	elif CONCT_ENABLED( CONCT_POINTER_32 )
		sprintf( buffer, "%016llx", *(uint64*)&value );
#	endif
		return DynamicString( buffer );
	}

	sint8 string_tools::parseSInt8( const char* pString )
	{
		return (sint8)atoi( pString );
	}

	sint16 string_tools::parseSInt16( const char* pString )
	{
		return (sint16)atoi( pString );
	}

	sint32 string_tools::parseSInt32( const char* pString )
	{
		return (sint32)atoi( pString );
	}

	sint64 string_tools::parseSInt64( const char* pString )
	{
		return (sint64)atoll( pString );
	}

	uint8 string_tools::parseUInt8( const char* pString )
	{
		return (uint8)atoi( pString );
	}

	uint16 string_tools::parseUInt16( const char* pString )
	{
		return (uint16)atoi( pString );
	}

	uint32 string_tools::parseUInt32( const char* pString )
	{
		return (uint32)atoi( pString );
	}

	uint64 string_tools::parseUInt64( const char* pString )
	{
		return (uint64)atoll( pString );
	}

	uint8 string_tools::parseUInt8Hex( const char* pString )
	{
		return (uint8)strtoll( pString, nullptr, 16 );
	}

	uint16 string_tools::parseUInt16Hex( const char* pString )
	{
		return (uint16)strtoll( pString, nullptr, 16 );
	}

	uint32 string_tools::parseUInt32Hex( const char* pString )
	{
		return (uint32)strtoll( pString, nullptr, 16 );
	}

	uint64 string_tools::parseUInt64Hex( const char* pString )
	{
		return (uint64)strtoll( pString, nullptr, 16 );
	}

	float string_tools::parseFloat( const char* pString )
	{
		return (float)atof( pString );
	}

	double string_tools::parseDouble( const char* pString )
	{
		return atof( pString );
	}

	bool string_tools::tryParseSInt8( sint8& target, const char* pString )
	{
		char* pEnd = nullptr;
		const sint64 result = strtoll( pString, &pEnd, 10 );
		if( pEnd == pString || !rangeCheckCast( target, result ) )
		{
			return false;
		}

		return true;
	}

	bool string_tools::tryParseSInt16( sint16& target, const char* pString )
	{
		char* pEnd = nullptr;
		const sint64 result = strtoll( pString, &pEnd, 10 );
		if( pEnd == pString || !rangeCheckCast( target, result ) )
		{
			return false;
		}

		return true;
	}

	bool string_tools::tryParseSInt32( sint32& target, const char* pString )
	{
		char* pEnd = nullptr;
		const sint64 result = strtoll( pString, &pEnd, 10 );
		if( pEnd == pString || !rangeCheckCast( target, result ) )
		{
			return false;
		}

		return true;
	}

	bool string_tools::tryParseSInt64( sint64& target, const char* pString )
	{
		char* pEnd = nullptr;
		const sint64 result = strtoll( pString, &pEnd, 10 );
		if( pEnd == pString || !rangeCheckCast( target, result ) )
		{
			return false;
		}

		return true;
	}

	bool string_tools::tryParseUInt8( uint8& target, const char* pString )
	{
		char* pEnd = nullptr;
		const uint64 result = (uint64)strtoll( pString, &pEnd, 10 );
		if( pEnd == pString || !rangeCheckCast( target, result ) )
		{
			return false;
		}

		return true;
	}

	bool string_tools::tryParseUInt16( uint16& target, const char* pString )
	{
		char* pEnd = nullptr;
		const uint64 result = (uint64)strtoll( pString, &pEnd, 10 );
		if( pEnd == pString || !rangeCheckCast( target, result ) )
		{
			return false;
		}

		return true;
	}

	bool string_tools::tryParseUInt32( uint32& target, const char* pString )
	{
		char* pEnd = nullptr;
		const uint64 result = (uint64)strtoll( pString, &pEnd, 10 );
		if( pEnd == pString || !rangeCheckCast( target, result ) )
		{
			return false;
		}

		return true;
	}

	bool string_tools::tryParseUInt64( uint64& target, const char* pString )
	{
		char* pEnd = nullptr;
		const uint64 result = (uint64)strtoll( pString, &pEnd, 10 );
		if( pEnd == pString || !rangeCheckCast( target, result ) )
		{
			return false;
		}

		return true;
	}

	bool string_tools::tryParseUInt8Hex( uint8& target, const char* pString )
	{
		char* pEnd = nullptr;
		const uint64 result = (uint64)strtoll( pString, &pEnd, 16 );
		if( pEnd == pString || !rangeCheckCast( target, result ) )
		{
			return false;
		}

		return true;
	}

	bool string_tools::tryParseUInt16Hex( uint16& target, const char* pString )
	{
		char* pEnd = nullptr;
		const uint64 result = (uint64)strtoll( pString, &pEnd, 16 );
		if( pEnd == pString || !rangeCheckCast( target, result ) )
		{
			return false;
		}

		return true;
	}

	bool string_tools::tryParseUInt32Hex( uint32& target, const char* pString )
	{
		char* pEnd = nullptr;
		const uint64 result = (uint64)strtoll( pString, &pEnd, 16 );
		if( pEnd == pString || !rangeCheckCast( target, result ) )
		{
			return false;
		}

		return true;
	}

	bool string_tools::tryParseUInt64Hex( uint64& target, const char* pString )
	{
		char* pEnd = nullptr;
		const uint64 result = (uint64)strtoll( pString, &pEnd, 16 );
		if( pEnd == pString || !rangeCheckCast( target, result ) )
		{
			return false;
		}

		return true;
	}

	bool string_tools::tryParseFloat( float& target, const char* pString )
	{
		char* pEnd = nullptr;
		const double result = strtof( pString, &pEnd );
		if( pEnd == pString || pEnd == pString )
		{
			return false;
		}

		target = (float)result;
		return true;
	}

	bool string_tools::tryParseDouble( double& target, const char* pString )
	{
		char* pEnd = nullptr;
		const double result = strtof( pString, &pEnd );
		if( pEnd == pString || pEnd == pString )
		{
			return false;
		}

		target = result;
		return true;
	}
}
