#pragma once

#include "conct_value_triats.h"

namespace conct
{
	template< class T >
	const T& Value::getStruct() const
	{
		TIKI_ASSERT( sizeof( T ) == getStructSize() );
		TIKI_ASSERT( getStructType() == ValueTypeTraits< T >::getTypeCrc() );
		return *(const T*)m_buffer.getData();
	}

	template< class T >
	ConstArrayView< T > Value::getArray() const
	{
		TIKI_ASSERT( sizeof( T ) == getArrayElementSize() );
		TIKI_ASSERT( getArrayType() == ValueTypeTraits< T >::getTypeCrc() );
		return m_buffer.cast< T >();
	}

	template< class T >
	void Value::setStruct( const T& value )
	{
		setStructData( &value, sizeof( value ), ValueTypeTraits< T >::getTypeCrc() );
	}

	template< class T >
	void Value::setArray( const ConstArrayView< T >& value )
	{
		setArrayData( value.getData(), (uint32)value.getElementSizeInBytes(), (uint32)value.getLength(), ValueTypeTraits< T >::getTypeCrc() );
	}
}