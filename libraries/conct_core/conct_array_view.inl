#pragma once

namespace conct
{
	template< class T >
	ArrayView<T>::ArrayView()
	{
		set( nullptr, 0u );
	}

	template< class T >
	ArrayView< T >::ArrayView( const T* pData, muint count )
	{
		set( pData, count );
	}

	template< class T >
	void ArrayView<T>::set( const T* pData, muint count )
	{
		m_pData	= pData;
		m_count	= count;
	}

	template< class T >
	const T& ArrayView< T >::operator[]( muint index ) const
	{
		CONCT_ASSERT( index < m_count );
		return m_pData[ index ];
	}
}
