#pragma once

#include "conct_number_limits.h"

namespace conct
{
	template< class T >
	ResultId ValueBuilder::setStruct( const T& value )
	{
		return setStruct( &value, sizeof( value ) );
	}

	template< class T >
	ResultId ValueBuilder::setArray( const ArrayView< T >& value )
	{
		return setArray( value.getData(), sizeof( T ), value.getCount() );
	}
}