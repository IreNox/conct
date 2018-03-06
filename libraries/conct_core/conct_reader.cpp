#include "conct_reader.h"

namespace conct
{
	Reader::Reader()
	{
		set( nullptr, 0u );
	}

	Reader::Reader( const void* pData, uint8_t size )
	{
		set( pData, size );
	}

	void Reader::set( const void* pData, uint8_t size )
	{
		m_pData			= static_cast< const uint8_t* >( pData );
		m_remainingSize	= size;
	}

	const void* Reader::readData( uint8_t length )
	{
		if( m_remainingSize < length )
		{
			return nullptr;
		}

		const void* pData = m_pData;

		m_pData += length;
		m_remainingSize -= length;

		return pData;
	}

	uint8_t Reader::readData( uint8_t* pTarget, uint8_t length )
	{
		uint8_t i = 0u;
		for( ; i < length && m_remainingSize > 0u; ++i )
		{
			*pTarget++ = *m_pData++;
			m_remainingSize--;
		}

		return i;
	}

	uint8_t Reader::readData( uint8_t* pTarget, uint8_t length, uint8_t remainingLength )
	{
		pTarget += ( length - remainingLength );

		uint8_t i = 0u;
		for( ; i < remainingLength && m_remainingSize > 0u; ++i )
		{
			*pTarget++ = *m_pData++;
			m_remainingSize--;
		}

		return i;
	}

	bool Reader::readByte( uint8_t& target )
	{
		return readData( &target, sizeof( target ) ) == sizeof( target );
	}

	uint8_t Reader::readShort( uint16_t& target )
	{
		return readData( (uint8_t*)&target, sizeof( target ) );
	}

	uint8_t Reader::readShort( uint16_t& target, uint8_t remainingLength )
	{
		return readData( ( uint8_t* )&target, sizeof( target ), remainingLength );
	}
}
