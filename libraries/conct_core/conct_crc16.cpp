#include "conct_crc16.h"

namespace conct
{
	// source: https://stackoverflow.com/questions/10564491/function-to-calculate-a-crc16-checksum
	uint16 calculateCrc16( const void* pData, uintreg size )
	{
		static const uint16 s_crc16 = 0x8005;

		const uint8* pBytes = static_cast< const uint8* >( pData );

		uint16 out = 0;
		int bits_read = 0, bit_flag;

		/* Sanity check: */
		if( pData == nullptr )
			return 0;

		while( size > 0 )
		{
			bit_flag = out >> 15;

			/* Get next bit: */
			out <<= 1;
			out |= ( *pBytes >> bits_read ) & 1; // item a) work from the least significant bits

			/* Increment bit counter: */
			bits_read++;
			if( bits_read > 7 )
			{
				bits_read = 0;
				pBytes++;
				size--;
			}

			/* Cycle check: */
			if( bit_flag )
				out ^= s_crc16;

		}

		// item b) "push out" the last 16 bits
		int i;
		for( i = 0; i < 16; ++i )
		{
			bit_flag = out >> 15;
			out <<= 1;
			if( bit_flag )
				out ^= s_crc16;
		}

		// item c) reverse the bits
		uint16 crc = 0;
		i = 0x8000;
		int j = 0x0001;
		for( ; i != 0; i >>=1, j <<= 1 )
		{
			if( i & out ) crc |= j;
		}

		return crc;
	}
}