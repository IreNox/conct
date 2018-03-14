#include "conct_reader.h"

namespace conct
{
	Reader::Reader()
	{
		set( nullptr, 0u );
	}

	Reader::Reader( const void* pData, uintreg size )
	{
		set( pData, size );
	}

	void Reader::set( const void* pData, uintreg size )
	{
		m_pData			= static_cast< const uint8* >( pData );
		m_remainingSize	= size;
	}

	uintreg Reader::readData( void* pTarget, uintreg length )
	{
		uint8* pTargetBytes = static_cast< uint8* >( pTarget );

		uintreg i = 0u;
		for( ; i < length && m_remainingSize > 0u; ++i )
		{
			*pTargetBytes++ = *m_pData++;
			m_remainingSize--;
		}

		return i;
	}

	uintreg Reader::readData( void* pTarget, uintreg length, uintreg alreadyRead )
	{
		uint8* pTargetBytes = static_cast< uint8* >( pTarget );

		pTargetBytes += alreadyRead;

		uintreg i = alreadyRead;
		for( ; i < length && m_remainingSize > 0u; ++i )
		{
			*pTargetBytes++ = *m_pData++;
			m_remainingSize--;
		}

		return i;
	}

	bool Reader::readByte( uint8& target )
	{
		return readData( &target, sizeof( target ) ) == sizeof( target );
	}

	uintreg Reader::readShort( uint16& target )
	{
		return readData( &target, sizeof( target ) );
	}

	uintreg Reader::readShort( uint16& target, uintreg alreadyRead )
	{
		return readData( &target, sizeof( target ), alreadyRead );
	}
}
