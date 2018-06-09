#pragma once

#include "conct_functions.h"

namespace conct
{
	template< class T >
	Vector< T >::Vector()
	{
		m_pData = nullptr;
		m_count = 0u;
		m_capacity = 0u;
	}

	template< class T >
	Vector< T >::Vector( const std::initializer_list< T >& initList )
	{
		m_pData = nullptr;
		m_count = 0u;
		m_capacity = 0u;

		reserve( initList.size() );
		for( const T& value : initList )
		{
			pushBack( value );
		}
	}

	template< class T >
	Vector< T >::~Vector()
	{
		delete[] m_pData;
	}

	template< class T >
	bool Vector< T >::isEmpty() const
	{
		return m_count == 0u;
	}

	template< class T >
	uintreg Vector< T >::getCount() const
	{
		return m_count;
	}

	template< class T >
	uintreg Vector< T >::getCapacity() const
	{
		return m_capacity;
	}

	template< class T >
	void Vector< T >::reserve( uintreg size )
	{
		checkCapacity( size );
	}

	template< class T >
	T& Vector< T >::pushBack()
	{
		checkCapacity( m_count + 1u );
		return m_pData[ m_count++ ];
	}

	template< class T >
	void Vector< T >::pushBack( const T& value )
	{
		checkCapacity( m_count + 1u );
		m_pData[ m_count++ ] = value;
	}

	template< class T >
	void conct::Vector<T>::pushRange( const T* pData, uintreg count )
	{
		checkCapacity( m_count + count );
		for( uintreg i = 0u; i < count; ++i )
		{
			m_pData[ m_count++ ] = pData[ i ];
		}
	}

	template< class T >
	void Vector< T >::popBack()
	{
		CONCT_ASSERT( m_count > 0u );
		m_count--;
	}

	template< class T >
	void Vector< T >::eraseSorted( const T& value )
	{
		eraseSorted( &value );
	}

	template< class T >
	void Vector< T >::eraseSorted( const T* pValue )
	{
		CONCT_ASSERT( pValue >= m_pData && pValue < m_pData + m_count );
		eraseSortedByIndex( pValue - m_pData );
	}

	template< class T >
	void Vector< T >::eraseSortedByIndex( uintreg index )
	{
		m_count--;
		for( uintreg i = index; i < m_count; ++i )
		{
			m_pData[ i ] = m_pData[ i + 1u ];
		}
	}

	template< class T >
	void Vector< T >::eraseUnsorted( const T& value )
	{
		eraseUnsorted( &value );
	}

	template< class T >
	void Vector< T >::eraseUnsorted( const T* pValue )
	{
		CONCT_ASSERT( pValue >= m_pData && pValue < m_pData + m_count );
		eraseUnsortedByIndex( pValue - m_pData );
	}

	template< class T >
	void Vector< T >::eraseUnsortedByIndex( uintreg index )
	{
		m_count--;
		if( index != m_count )
		{
			m_pData[ index ] = m_pData[ m_count ];
		}
	}

	template< class T >
	T* conct::Vector< T >::getData()
	{
		return m_pData;
	}

	template< class T >
	const T* conct::Vector< T >::getData() const
	{
		return m_pData;
	}

	template< class T >
	T& Vector< T >::operator[]( uintreg index )
	{
		CONCT_ASSERT( index < m_count );
		return m_pData[ index ];
	}

	template< class T >
	const T& Vector< T >::operator[]( uintreg index ) const
	{
		CONCT_ASSERT( index < m_count );
		return m_pData[ index ];
	}

	template< class T >
	void Vector< T >::checkCapacity( uintreg size )
	{
		const uintreg nextCapacity = getNextPowerOfTwo( CONCT_MAX( 1u, size ) );
		T* pNewData = new T[ nextCapacity ];
		CONCT_ASSERT( pNewData != nullptr );

		for( uintreg i = 0u; i < m_count; ++i )
		{
			pNewData[ i ] = m_pData[ i ];
		}

		delete[] m_pData;
		m_pData = pNewData;
		m_capacity = nextCapacity;
	}
}
