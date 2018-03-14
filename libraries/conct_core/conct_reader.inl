#pragma once

namespace conct
{
	template< class T >
	uintreg Reader::readStruct( T& target )
	{
		return readData( &target, sizeof( T ) );
	}

	template< class T >
	uintreg Reader::readStruct( T& target, uintreg alreadyRead )
	{
		return readData( &target, sizeof( T ), alreadyRead );
	}
}
