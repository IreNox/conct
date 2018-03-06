#include "conct_writer.h"

namespace conct
{
	Writer::Writer()
	{
		set( nullptr, 0u );
	}

	Writer::Writer( void* pData, uint8_t size )
	{
		set( pData, size );
	}

	void Writer::set( void* pData, uint8_t size )
	{
		m_pData			= static_cast< uint8_t* >( pData );
		m_remainingSize	= size;
	}

	bool Writer::writeData( const uint8_t* pData, uint8_t length )
	{
		if( m_remainingSize < length )
		{
			return false;
		}

		while( length-- )
		{
			*m_pData++ = *pData++;
			m_remainingSize--;
		}

		return true;
	}

	bool Writer::writeByte( uint8_t data )
	{
		if( m_remainingSize < sizeof( data ) )
		{
			return false;
		}

		*m_pData++ = data;
		m_remainingSize--;

		return true;
	}
}
