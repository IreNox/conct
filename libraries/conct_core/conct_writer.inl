#pragma once

namespace conct
{
	template< class T >
	bool Writer::writeStruct( const T& data )
	{
		return writeData( static_cast< const uint8_t* >( &data ), sizeof( data ) );
	}
}
