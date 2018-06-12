#pragma once

namespace conct
{
	template< class T >
	ArrayView<T>::ArrayView()
	{
		set( nullptr, 0u );
	}

	template< class T >
	ArrayView< T >::ArrayView( const T* pData, uintreg length )
	{
		set( pData, length );
	}

	template< class T >
	void ArrayView<T>::set( const T* pData, uintreg length )
	{
		m_pData		= pData;
		m_length	= length;
	}

	template< class T >
	const T& ArrayView< T >::operator[]( uintreg index ) const
	{
		CONCT_ASSERT( index < m_length );
		return m_pData[ index ];
	}
}
