#pragma once

namespace conct
{
	template< class T >
	Array<T>::Array()
	{
		set( nullptr, 0u );
	}

	template< class T >
	Array< T >::Array( T* pData, uintreg count )
	{
		set( pData, count );
	}

	template< class T >
	void Array<T>::set( T* pData, uintreg count )
	{
		m_pData	= pData;
		m_count	= count;
	}

	template< class T >
	const T& Array< T >::operator[]( uintreg index ) const
	{
		CONCT_ASSERT( index < m_count );
		return m_pData[ index ];
	}
}
