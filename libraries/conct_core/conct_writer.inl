#pragma once

namespace conct
{
	template< class T >
	muint Writer::writeStruct( const T& data )
	{
		return writeData( &data, sizeof( data ) );
	}

	template< class T >
	muint Writer::writeStruct( const T& data, muint remainingLength )
	{
		return writeData( &data, sizeof( data ), remainingLength );
	}
}
