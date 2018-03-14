#pragma once

namespace conct
{
	template< class T >
	ArrayView<T>::ArrayView()
	{
		set( nullptr, 0u );
	}

	template< class T >
	ArrayView< T >::ArrayView( const T* pData, uintreg count )
	{
		set( pData, count );
	}

	template< class T >
	void ArrayView<T>::set( const T* pData, uintreg count )
	{
		m_pData	= pData;
		m_count	= count;
	}

	template< class T >
	const T& ArrayView< T >::operator[]( uintreg index ) const
	{
		CONCT_ASSERT( index < m_count );
		return m_pData[ index ];
	}
}
