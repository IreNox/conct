#pragma once

#include "conct_functions.h"

namespace conct
{
	template< class T >
	Queue< T >::Queue()
		: m_pData( nullptr ), m_top( 0u ), m_bottom( 0u ), m_length( 0u ), m_capacity( 0u )
	{
	}

	template< class T >
	Queue< T >::Queue( const Queue& rhs )
		: m_pData( nullptr ), m_top( 0u ), m_bottom( 0u ), m_length( 0u ), m_capacity( 0u )
	{
		*this = rhs;
	}

	template< class T >
	Queue< T >::Queue( const std::initializer_list< T >& initList )
		: m_pData( nullptr ), m_top( 0u ), m_bottom( 0u ), m_length( 0u ), m_capacity( 0u )
	{
		pushRange( initList.begin(), initList.size() );
	}

	template< class T >
	Queue< T >::~Queue()
	{
		delete[] m_pData;
	}

	template< class T >
	bool Queue< T >::isEmpty() const
	{
		return m_length == 0u;
	}

	template< class T >
	uintreg Queue< T >::getLength() const
	{
		return m_length;
	}

	template< class T >
	uintreg Queue< T >::getCapacity() const
	{
		return m_capacity;
	}

	template< class T >
	void Queue<T>::clear()
	{
		m_top		= 0u;
		m_bottom	= 0u;
		m_length	= 0u;
	}

	template< class T >
	void Queue< T >::reserve( uintreg size )
	{
		checkCapacity( size );
	}

	template< class T >
	T& Queue< T >::pushBack()
	{
		checkCapacity( m_length + 1u );

		const uintreg index = m_bottom;
		m_bottom = ( m_bottom + 1u ) % m_capacity;
		m_length++;
		return m_pData[ index ];
	}

	template< class T >
	void Queue< T >::pushBack( const T& value )
	{
		checkCapacity( m_length + 1u );

		const uintreg index = m_bottom;
		m_bottom = ( m_bottom + 1u ) % m_capacity;
		m_length++;
		m_pData[ index ] = value;
	}

	template< class T >
	void Queue<T>::pushRange( const Queue< T >& vector )
	{
		pushRange( vector.getData(), vector.getLength() );
	}

	template< class T >
	void Queue<T>::pushRange( const ArrayView< T >& arrayView )
	{
		pushRange( arrayView.getData(), arrayView.getLength() );
	}

	template< class T >
	void Queue<T>::pushRange( const T* pData, uintreg length )
	{
		checkCapacity( m_length + length );
		for( uintreg i = 0u; i < length; ++i )
		{
			const uintreg index = m_bottom;
			m_bottom = ( m_bottom + 1u ) % m_capacity;
			m_pData[ index ] = pData[ i ];
		}
		m_length += length;
	}

	template< class T >
	void Queue< T >::popFront()
	{
		CONCT_ASSERT( m_length > 0u );
		m_top = ( m_top + 1u ) % m_capacity;
		m_length--;
	}

	template< class T >
	bool Queue<T>::tryPopFront( T& target )
	{
		if( isEmpty() )
		{
			return false;
		}

		target = m_pData[ m_top ];
		m_top = ( m_top + 1u ) % m_capacity;
		m_length--;
		return true;
	}

	template< class T >
	T& Queue<T>::getFront()
	{
		CONCT_ASSERT( m_length > 0u );
		return m_pData[ m_top ];
	}

	template< class T >
	const T& Queue<T>::getFront() const
	{
		CONCT_ASSERT( m_length > 0u );
		return m_pData[ m_top ];
	}

	template< class T >
	Queue< T >& Queue< T >::operator=( const Queue& rhs )
	{
		clear();
		pushRange( rhs.getData(), rhs.getLength() );
		return *this;
	}

	template< class T >
	void Queue< T >::checkCapacity( uintreg capacity )
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
