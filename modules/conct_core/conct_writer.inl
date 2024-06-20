#pragma once

namespace conct
{
	template< class T >
	uintreg Writer::writeStruct( const T& data )
	{
		return writeData( &data, sizeof( data ) );
	}

	template< class T >
	uintreg Writer::writeStruct( const T& data, uintreg remainingLength )
	{
		return writeData( &data, sizeof( data ), remainingLength );
	}
}
