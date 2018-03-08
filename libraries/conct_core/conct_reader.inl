#pragma once

namespace conct
{
	template< class T >
	muint Reader::readStruct( T& target )
	{
		return readData( &target, sizeof( T ) );
	}

	template< class T >
	muint Reader::readStruct( T& target, muint alreadyRead )
	{
		return readData( &target, sizeof( T ), alreadyRead );
	}
}
