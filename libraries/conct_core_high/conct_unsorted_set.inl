#pragma once

#include "conct_functions.h"

namespace conct
{
	template< class T >
	UnsortedSet< T >::UnsortedSet()
	{
		m_pData = nullptr;
		m_count = 0u;
		m_capacity = 0u;
	}

	template< class T >
	UnsortedSet< T >::UnsortedSet( const UnsortedSet& rhs )
	{
		*this = rhs;
	}

	template< class T >
	UnsortedSet< T >::UnsortedSet( const std::initializer_list< T >& initList )
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
	UnsortedSet< T >::~UnsortedSet()
	{
		delete[] m_pData;
	}

	template< class T >
	bool UnsortedSet< T >::isEmpty() const
	{
		return m_count == 0u;
	}

	template< class T >
	uintreg UnsortedSet< T >::getCount() const
	{
		return m_count;
	}

	template< class T >
	uintreg UnsortedSet< T >::getCapacity() const
	{
		return m_capacity;
	}

	template< class T >
	void conct::UnsortedSet<T>::clear()
	{
		m_count = 0u;
	}

	template< class T >
	void UnsortedSet< T >::reserve( uintreg size )
	{
		checkCapacity( size );
	}

	template< class T >
	void UnsortedSet< T >::eraseSorted( const T& value )
	{
		eraseSorted( &value );
	}

	template< class T >
	void UnsortedSet< T >::eraseSorted( const T* pValue )
	{
		CONCT_ASSERT( pValue >= m_pData && pValue < m_pData + m_count );
		eraseSortedByIndex( pValue - m_pData );
	}

	template< class T >
	void UnsortedSet< T >::eraseSortedByIndex( uintreg index )
	{
		m_count--;
		for( uintreg i = index; i < m_count; ++i )
		{
			m_pData[ i ] = m_pData[ i + 1u ];
		}
	}

	template< class T >
	void UnsortedSet< T >::eraseUnsorted( const T& value )
	{
		eraseUnsorted( &value );
	}

	template< class T >
	void UnsortedSet< T >::eraseUnsorted( const T* pValue )
	{
		CONCT_ASSERT( pValue >= m_pData && pValue < m_pData + m_count );
		eraseUnsortedByIndex( pValue - m_pData );
	}

	template< class T >
	void UnsortedSet< T >::eraseUnsortedByIndex( uintreg index )
	{
		m_count--;
		if( index != m_count )
		{
			m_pData[ index ] = m_pData[ m_count ];
		}
	}

	template< class T >
	T* conct::UnsortedSet< T >::getData()
	{
		return m_pData;
	}

	template< class T >
	const T* conct::UnsortedSet< T >::getData() const
	{
		return m_pData;
	}

	template< class T >
	T& conct::UnsortedSet<T>::getFirst()
	{
		CONCT_ASSERT( m_count > 0u );
		return m_pData[ 0u ];
	}

	template< class T >
	const T& conct::UnsortedSet<T>::getFirst() const
	{
		CONCT_ASSERT( m_count > 0u );
		return m_pData[ 0u ];
	}

	template< class T >
	T& conct::UnsortedSet<T>::getLast()
	{
		CONCT_ASSERT( m_count > 0u );
		return m_pData[ m_count - 1u ];
	}

	template< class T >
	const T& conct::UnsortedSet<T>::getLast() const
	{
		CONCT_ASSERT( m_count > 0u );
		return m_pData[ m_count - 1u ];
	}

	template< class T >
	UnsortedSet< T >& UnsortedSet< T >::operator=( const UnsortedSet& rhs )
	{
		clear();
		pushRange( rhs.getData(), rhs.getCount() );
		return *this;
	}

	template< class T >
	T& UnsortedSet< T >::operator[]( uintreg index )
	{
		CONCT_ASSERT( index < m_count );
		return m_pData[ index ];
	}

	template< class T >
	const T& UnsortedSet< T >::operator[]( uintreg index ) const
	{
		CONCT_ASSERT( index < m_count );
		return m_pData[ index ];
	}

	template< class T >
	void UnsortedSet< T >::checkCapacity( uintreg size )
	{
		const uintreg nextCapacity = getNextPowerOfTwo( size );
		if( nextCapacity < m_capacity )
		{
			return;
		}

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
