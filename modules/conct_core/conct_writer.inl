#pragma once

namespace conct
{
	template< class T >
	bool BinaryWriter::writeStruct( const T& data )
	{
		return writeData( &data, sizeof( data ) );
	}

	template< class T >
	uintreg BinaryWriter::writeStruct( const T& data, uintreg remainingLength )
	{
		return writeData( &data, sizeof( data ), remainingLength );
	}
}
