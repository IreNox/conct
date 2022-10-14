#pragma once

namespace conct
{
	template< class T >
	Array< T >::Array()
		: m_pData( nullptr )
		, m_length( 0u )
	{
	}

	template< class T >
	Array< T >::Array( T* pData, uintreg length )
		: m_pData( pData )
		, m_length( length )
	{
	}

	template< class T >
	T& Array< T >::getFront()
	{
		CONCT_ASSERT( m_length > 0u );
		return m_pData[ 0u ];
	}

	template< class T >
	const T& Array< T >::getFront() const
	{
		CONCT_ASSERT( m_length > 0u );
		return m_pData[ 0u ];
	}

	template< class T >
	T& Array< T >::getBack()
	{
		CONCT_ASSERT( m_length > 0u );
		return m_pData[ m_length - 1u ];
	}

	template< class T >
	const T& Array< T >::getBack() const
	{
		CONCT_ASSERT( m_length > 0u );
		return m_pData[ m_length - 1u ];
	}

	template< class T >
	ArrayView< T > Array< T >::toView() const
	{
		return ArrayView< T >( m_pData, m_length );
	}

	template< class T >
	T& conct::Array<T>::getElement( uintreg index )
	{
		CONCT_ASSERT( index < m_length );
		return m_pData[ index ];
	}

	template< class T >
	const T& conct::Array<T>::getElement( uintreg index ) const
	{
		CONCT_ASSERT( index < m_length );
		return m_pData[ index ];
	}

	template< class T >
	T& conct::Array<T>::getReverseElement( uintreg index )
	{
		CONCT_ASSERT( index < m_length );
		return m_pData[ (m_length - 1u) - index ];
	}

	template< class T >
	const T& conct::Array<T>::getReverseElement( uintreg index ) const
	{
		CONCT_ASSERT( index < m_length );
		return m_pData[ (m_length - 1u) - index ];
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
