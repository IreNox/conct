#pragma once

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
	T* DataBuilder::pushArray( uintreg count )
	{
		return static_cast< T* >( pushData( count * sizeof( T ) ) );
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
