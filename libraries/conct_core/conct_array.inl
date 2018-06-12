#pragma once

namespace conct
{
	template< class T >
	Array< T >::Array()
	{
		clear();
	}

	template< class T >
	Array< T >::Array( T* pData, uintreg length )
	{
		set( pData, length );
	}

	template< class T >
	void Array< T >::set( T* pData, uintreg length )
	{
		m_pData		= pData;
		m_length	= length;
	}

	template< class T >
	void Array< T >::clear()
	{
		set( nullptr, 0u );
	}

	template< class T >
	T& Array< T >::operator[]( uintreg index )
	{
		CONCT_ASSERT( index < m_length );
		return m_pData[ index ];
	}

	template< class T >
	const T& Array< T >::operator[]( uintreg index ) const
	{
		CONCT_ASSERT( index < m_length );
		return m_pData[ index ];
	}
}
