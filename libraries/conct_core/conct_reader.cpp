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

	bool Reader::readData( uint8_t* pTarget, uint8_t length )
	{
		if( m_remainingSize < length )
		{
			return false;
		}

		while( length-- )
		{
			*pTarget++ = *m_pData++;
			m_remainingSize--;
		}

		return true;
	}

	bool Reader::readByte( uint8_t& target )
	{
		if( m_remainingSize < sizeof( target ) )
		{
			return false;
		}

		target = *m_pData;

		m_pData++;
		m_remainingSize--;

		return true;
	}
}
