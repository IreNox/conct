#pragma once

#include "conct_value_triats.h"

namespace conct
{
	template< class T >
	const T& ValueHigh::getStruct() const
	{
		TIKI_ASSERT( sizeof( T ) == getStructSize() );
		TIKI_ASSERT( getStructType() == ValueTypeTraits< T >::getTypeCrc() );
		return *(const T*)m_buffer.getData();
	}

	template< class T >
	ConstArrayView< T > ValueHigh::getArray() const
	{
		TIKI_ASSERT( sizeof( T ) == getArrayElementSize() );
		TIKI_ASSERT( getArrayType() == ValueTypeTraits< T >::getTypeCrc() );
		return ConstArrayView< T >( (const T*)m_buffer.getData(), getArrayLength() );
	}
}