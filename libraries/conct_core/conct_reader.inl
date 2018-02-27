#pragma once

namespace conct
{
	template< class T >
	const T* Reader::readStruct()
	{
		return static_cast< const T* >( readData( sizeof( T ) ) );
	}
}
