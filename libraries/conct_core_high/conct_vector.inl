#pragma once

#include "conct_functions.h"

namespace conct
{
	template< class T >
	Vector< T >::Vector()
		: m_pData( nullptr ), m_length( 0u ), m_capacity( 0u )
	{
	}

	template< class T >
	Vector< T >::Vector( const Vector& rhs )
	{
		*this = rhs;
	}

	template< class T >
	Vector< T >::Vector( const std::initializer_list< T >& initList )
		: m_pData( nullptr ), m_length( 0u ), m_capacity( 0u )
	{
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
		return m_length == 0u;
	}

	template< class T >
	uintreg Vector< T >::getLength() const
	{
		return m_length;
	}

	template< class T >
	uintreg Vector< T >::getCapacity() const
	{
		return m_capacity;
	}

	template< class T >
	void Vector<T>::clear()
	{
		m_length = 0u;
	}

	template< class T >
	void Vector< T >::reserve( uintreg size )
	{
		checkCapacity( size );
	}

	template< class T >
	void Vector<T>::setLength( uintreg size )
	{
		checkCapacity( size );
		m_length = size;
	}

	template< class T >
	T& Vector< T >::pushBack()
	{
		checkCapacity( m_length + 1u );
		return m_pData[ m_length++ ];
	}

	template< class T >
	void Vector< T >::pushBack( const T& value )
	{
		checkCapacity( m_length + 1u );
		m_pData[ m_length++ ] = value;
	}

	template< class T >
	void Vector<T>::pushRange( const T* pData, uintreg length )
	{
		checkCapacity( m_length + length );
		for( uintreg i = 0u; i < length; ++i )
		{
			m_pData[ m_length++ ] = pData[ i ];
		}
	}

	template< class T >
	void Vector< T >::popBack()
	{
		CONCT_ASSERT( m_length > 0u );
		m_length--;
	}

	template< class T >
	void Vector< T >::eraseSorted( const T& value )
	{
		eraseSorted( &value );
	}

	template< class T >
	void Vector< T >::eraseSorted( const T* pValue )
	{
		CONCT_ASSERT( pValue >= m_pData && pValue < m_pData + m_length );
		eraseSortedByIndex( pValue - m_pData );
	}

	template< class T >
	void Vector< T >::eraseSortedByIndex( uintreg index )
	{
		m_length--;
		for( uintreg i = index; i < m_length; ++i )
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
		CONCT_ASSERT( pValue >= m_pData && pValue < m_pData + m_length );
		eraseUnsortedByIndex( pValue - m_pData );
	}

	template< class T >
	void Vector< T >::eraseUnsortedByIndex( uintreg index )
	{
		m_length--;
		if( index != m_length )
		{
			m_pData[ index ] = m_pData[ m_length ];
		}
	}

	template< class T >
	T* Vector< T >::getData()
	{
		return m_pData;
	}

	template< class T >
	const T* Vector< T >::getData() const
	{
		return m_pData;
	}

	template< class T >
	T* Vector<T>::getBegin()
	{
		return m_pData;
	}

	template< class T >
	const T* Vector<T>::getBegin() const
	{
		return m_pData;
	}

	template< class T >
	T* Vector<T>::getEnd()
	{
		return m_pData + m_length;
	}

	template< class T >
	const T* Vector<T>::getEnd() const
	{
		return m_pData + m_length;
	}

	template< class T >
	T& Vector<T>::getFirst()
	{
		CONCT_ASSERT( m_length > 0u );
		return m_pData[ 0u ];
	}

	template< class T >
	const T& Vector<T>::getFirst() const
	{
		CONCT_ASSERT( m_length > 0u );
		return m_pData[ 0u ];
	}

	template< class T >
	T& Vector<T>::getLast()
	{
		CONCT_ASSERT( m_length > 0u );
		return m_pData[ m_length - 1u ];
	}

	template< class T >
	const T& Vector<T>::getLast() const
	{
		CONCT_ASSERT( m_length > 0u );
		return m_pData[ m_length - 1u ];
	}

	template< class T >
	Vector< T >& Vector< T >::operator=( const Vector& rhs )
	{
		clear();
		pushRange( rhs.getData(), rhs.getLength() );
		return *this;
	}

	template< class T >
	T& Vector< T >::operator[]( uintreg index )
	{
		CONCT_ASSERT( index < m_length );
		return m_pData[ index ];
	}

	template< class T >
	const T& Vector< T >::operator[]( uintreg index ) const
	{
		CONCT_ASSERT( index < m_length );
		return m_pData[ index ];
	}

	template< class T >
	void Vector< T >::checkCapacity( uintreg capacity )
	{
		const uintreg nextCapacity = getNextPowerOfTwo( capacity );
		if( nextCapacity < m_capacity )
		{
			return;
		}

		T* pNewData = new T[ nextCapacity ];
		CONCT_ASSERT( pNewData != nullptr );

		for( uintreg i = 0u; i < m_length; ++i )
		{
			pNewData[ i ] = m_pData[ i ];
		}

		delete[] m_pData;
		m_pData = pNewData;
		m_capacity = nextCapacity;
	}
}
