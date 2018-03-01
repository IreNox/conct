#pragma once

namespace conct
{
	template< class T >
	Array<T>::Array()
	{
		set( nullptr, 0u );
	}

	template< class T >
	Array< T >::Array( T* pData, uint8_t count )
	{
		set( pData, count );
	}

	template< class T >
	void Array<T>::set( T* pData, uint8_t count )
	{
		m_pData	= pData;
		m_count	= count;
	}

	template< class T >
	const T& Array< T >::operator[]( uint8_t index ) const
	{
		return m_pData[ index ];
	}
}
