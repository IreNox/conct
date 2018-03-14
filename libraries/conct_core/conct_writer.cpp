#include "conct_writer.h"

namespace conct
{
	Writer::Writer()
	{
		set( nullptr, 0u );
	}

	Writer::Writer( void* pData, uintreg size )
	{
		set( pData, size );
	}

	void Writer::set( void* pData, uintreg size )
	{
		m_pData			= static_cast< uint8* >( pData );
		m_remainingSize	= size;
	}

	uintreg Writer::writeData( const void* pData, uintreg length )
	{
		const uint8* pDataBytes = static_cast< const uint8* >( pData );

		uintreg i = 0u;
		for( ; i < length && m_remainingSize > 0u; ++i )
		{
			*m_pData++ = *pDataBytes++;
			m_remainingSize--;
		}

		return i;
	}

	uintreg Writer::writeData( const void* pData, uintreg length, uintreg remainingLength )
	{
		const uint8* pDataBytes = static_cast< const uint8* >( pData );
		pDataBytes += ( length - remainingLength );

		uintreg i = 0u;
		for( ; i < remainingLength && m_remainingSize > 0u; ++i )
		{
			*m_pData++ = *pDataBytes++;
			m_remainingSize--;
		}

		return i;
	}

	bool Writer::writeByte( uint8 data )
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
