#include "conct_reader.h"

namespace conct
{
	Reader::Reader()
	{
		set( nullptr, 0u );
	}

	Reader::Reader( const void* pData, muint size )
	{
		set( pData, size );
	}

	void Reader::set( const void* pData, muint size )
	{
		m_pData			= static_cast< const uint8_t* >( pData );
		m_remainingSize	= size;
	}

	uint8_t Reader::readData( void* pTarget, muint length )
	{
		uint8_t* pTargetBytes = static_cast< uint8_t* >( pTarget );

		muint i = 0u;
		for( ; i < length && m_remainingSize > 0u; ++i )
		{
			*pTargetBytes++ = *m_pData++;
			m_remainingSize--;
		}

		return i;
	}

	muint Reader::readData( void* pTarget, muint length, muint alreadyRead )
	{
		uint8_t* pTargetBytes = static_cast< uint8_t* >( pTarget );

		pTargetBytes += alreadyRead;

		muint i = alreadyRead;
		for( ; i < length && m_remainingSize > 0u; ++i )
		{
			*pTargetBytes++ = *m_pData++;
			m_remainingSize--;
		}

		return i;
	}

	bool Reader::readByte( uint8_t& target )
	{
		return readData( &target, sizeof( target ) ) == sizeof( target );
	}

	muint Reader::readShort( uint16_t& target )
	{
		return readData( &target, sizeof( target ) );
	}

	muint Reader::readShort( uint16_t& target, muint alreadyRead )
	{
		return readData( &target, sizeof( target ), alreadyRead );
	}
}
