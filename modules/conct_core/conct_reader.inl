#pragma once

namespace conct
{
	template< class T >
	bool BinaryReader::readStruct( T& target )
	{
		return readData( &target, sizeof( T ) ) == sizeof( T );
	}

	template< class T >
	uintreg BinaryReader::readStruct( T& target, uintreg alreadyRead )
	{
		return readData( &target, sizeof( T ), alreadyRead );
	}
}
