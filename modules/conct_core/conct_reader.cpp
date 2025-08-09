#include "conct_reader.h"

namespace conct
{
	BinaryReader::BinaryReader()
	{
		set( nullptr, 0u );
	}

	BinaryReader::BinaryReader( const ConstArrayView< byte >& data )
	{
		set( data.getData(), data.getLength() );
	}

	BinaryReader::BinaryReader( const void* pData, uintreg size )
	{
		set( pData, size );
	}

	void BinaryReader::set( const void* pData, uintreg size )
	{
		m_pData			= static_cast< const uint8* >( pData );
		m_remainingSize	= size;
	}

	uintreg BinaryReader::readData( void* pTarget, uintreg length )
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

	uintreg BinaryReader::readData( void* pTarget, uintreg length, uintreg alreadyRead )
	{
		uint8* pTargetBytes = static_cast< uint8* >( pTarget );

		pTargetBytes += alreadyRead;

		uintreg i = alreadyRead;
		for( ; i < length && m_remainingSize > 0u; ++i )
		{
			*pTargetBytes++ = *m_pData++;
			m_remainingSize--;
		}

		return i - alreadyRead;
	}

	bool BinaryReader::readByte( byte& target )
	{
		return readData( &target, sizeof( target ) ) == sizeof( target );
	}

	bool BinaryReader::readShort( uint16& target )
	{
		return readData( &target, sizeof( target ) ) == sizeof( target );
	}

	uintreg BinaryReader::readShort( uint16& target, uintreg alreadyRead )
	{
		return readData( &target, sizeof( target ), alreadyRead );
	}
}
