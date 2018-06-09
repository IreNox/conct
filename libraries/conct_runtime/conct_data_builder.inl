#pragma once

#include "conct_value_builder.h"

namespace conct
{
	template< class T >
	T* DataBuilder::pushStruct()
	{
		return static_cast< T* >( pushData( sizeof( T ) ) );
	}

	template< class T >
	void DataBuilder::pushStruct( const T& data )
	{
		pushData( &data, sizeof( data ) );
	}

	template< class T >
	Array< T > DataBuilder::pushArray( uintreg count )
	{
		T* pData = static_cast< T* >( pushData( count * sizeof( T ) ) );
		if( pData == nullptr )
		{
			return Array< T >();
		}

		return Array< T >(  pData, count );
	}

	template< class T >
	void DataBuilder::pushValueData( Value* pTargetValue, const T* pSourceValue )
	{
		ValueBuilder valueBuilder( this, pTargetValue );
		valueBuilder.setValue( pSourceValue );
	}

	template< uintreg TBufferSize >
	BufferedDataBuilder< TBufferSize >::BufferedDataBuilder()
		: DataBuilder( m_buffer, sizeof( m_buffer ) )
	{
	}

	template< class T >
	StructDataBuilder< T >::StructDataBuilder()
	{
	}

	template< class T >
	T* StructDataBuilder< T >::pushGenericStruct()
	{
		return DataBuilder::pushStruct< T >();
	}
}
