#pragma once

#include "conct_value_triats.h"

namespace conct
{
	template< class T >
	const T& ValueHigh::getStruct() const
	{
		CONCT_ASSERT( sizeof( T ) == getStructSize() );
		CONCT_ASSERT( getStructType() == ValueTypeTraits< T >::getTypeCrc() );
		return *( const T* )m_buffer.getData();
	}

	template< class T >
	ArrayView< T > ValueHigh::getArray() const
	{
		CONCT_ASSERT( sizeof( T ) == getArrayElementSize() );
		CONCT_ASSERT( getArrayType() == ValueTypeTraits< T >::getTypeCrc() );
		return ArrayView< T >( ( const T* )m_buffer.getData(), getArrayCount() );
	}
}