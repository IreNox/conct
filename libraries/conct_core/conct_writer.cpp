#include "conct_writer.h"

namespace conct
{
	Writer::Writer()
	{
		set( nullptr, 0u );
	}

	Writer::Writer( void* pData, muint size )
	{
		set( pData, size );
	}

	void Writer::set( void* pData, muint size )
	{
		m_pData			= static_cast< uint8_t* >( pData );
		m_remainingSize	= size;
	}

	muint Writer::writeData( const void* pData, muint length )
	{
		const uint8_t* pDataBytes = static_cast< const uint8_t* >( pData );

		muint i = 0u;
		for( ; i < length && m_remainingSize > 0u; ++i )
		{
			*m_pData++ = *pDataBytes++;
			m_remainingSize--;
		}

		return i;
	}

	muint Writer::writeData( const void* pData, muint length, muint remainingLength )
	{
		const uint8_t* pDataBytes = static_cast< const uint8_t* >( pData );
		pDataBytes += ( length - remainingLength );

		muint i = 0u;
		for( ; i < remainingLength && m_remainingSize > 0u; ++i )
		{
			*m_pData++ = *pDataBytes++;
			m_remainingSize--;
		}

		return i;
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
