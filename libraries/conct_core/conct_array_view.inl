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
	constexpr ArrayView< T >::ArrayView( const std::initializer_list< T >& initList )
		: m_pData( initList.begin() )
		, m_length( initList.size() )
	{
	}

	template< class T >
	constexpr void ArrayView<T>::set( const T* pData, uintreg length )
	{
		m_pData		= pData;
		m_length	= length;
	}

	template< class T >
	const T* ArrayView<T>::getBegin() const
	{
		return m_pData;
	}

	template< class T >
	const T* ArrayView<T>::getEnd() const
	{
		return m_pData + m_length;
	}

	template< class T >
	const T& ArrayView<T>::getFront() const
	{
		CONCT_ASSERT( m_length > 0u );
		return m_pData[ 0u ];
	}

	template< class T >
	const T& ArrayView<T>::getBack() const
	{
		CONCT_ASSERT( m_length > 0u );
		return m_pData[ m_length - 1u ];
	}

	template< class T >
	const T& ArrayView< T >::operator[]( uintreg index ) const
	{
		CONCT_ASSERT( index < m_length );
		return m_pData[ index ];
	}
}
