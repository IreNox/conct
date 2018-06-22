#pragma once

#include "conct_functions.h"

namespace conct
{
	template< class TKey, class TValue >
	Map< TKey, TValue >::Map()
		: m_pData( nullptr ), m_length( 0u ), m_capacity( 0u )
	{
	}

	template< class TKey, class TValue >
	Map< TKey, TValue >::Map( const Map& rhs )
		: m_pData( nullptr ), m_length( 0u ), m_capacity( 0u )
	{
		*this = rhs;
	}

	template< class TKey, class TValue >
	Map< TKey, TValue >::Map( const std::initializer_list< PairType >& initList )
		: m_pData( nullptr ), m_length( 0u ), m_capacity( 0u )
	{
		for( const PairType& pair : initList )
		{
			insert( pair.key, pair.value );
		}
	}

	template< class TKey, class TValue >
	Map< TKey, TValue >::~Map()
	{
		delete[] m_pData;
	}

	template< class TKey, class TValue >
	bool Map< TKey, TValue >::isEmpty() const
	{
		return m_length == 0u;
	}

	template< class TKey, class TValue >
	uintreg Map< TKey, TValue >::getLength() const
	{
		return m_length;
	}

	template< class TKey, class TValue >
	uintreg Map< TKey, TValue >::getCapacity() const
	{
		return m_capacity;
	}

	template< class TKey, class TValue >
	void Map< TKey, TValue >::clear()
	{
		m_length = 0u;
	}

	template< class TKey, class TValue >
	void Map< TKey, TValue >::reserve( uintreg size )
	{
		checkCapacity( size );
	}

	template< class TKey, class TValue >
	bool Map<TKey, TValue>::find( TValue& target, const TKey& key )
	{
		const uintreg index = findIndex( key );
		if( index == ( uintreg )-1 )
		{
			return false;
		}

		target = m_pData[ index ].value;
		return true;
	}

	template< class TKey, class TValue >
	bool Map<TKey, TValue>::insert( const TKey& key, const TValue& value )
	{
		PairType* pPair = nullptr;
		const bool result = findOrInsertPair( &pPair, key );
		pPair->value = value;
		return result;
	}

	template< class TKey, class TValue >
	bool Map<TKey, TValue>::erase( const TKey& key )
	{
		const uint index = findIndex( key );
		if( index == ( uintreg )-1 )
		{
			return false;
		}

		m_length--;
		for( uint i = index; i < m_length; ++i )
		{
			m_pData[ i ] = m_pData[ i + 1u ];
		}

		return true;
	}

	template< class TKey, class TValue >
	Pair< TKey, TValue >* Map< TKey, TValue >::getData()
	{
		return m_pData;
	}

	template< class TKey, class TValue >
	const Pair< TKey, TValue >* Map< TKey, TValue >::getData() const
	{
		return m_pData;
	}

	template< class TKey, class TValue >
	Pair< TKey, TValue >* Map< TKey, TValue >::getBegin()
	{
		return m_pData;
	}

	template< class TKey, class TValue >
	const Pair< TKey, TValue >* Map< TKey, TValue >::getBegin() const
	{
		return m_pData;
	}

	template< class TKey, class TValue >
	Pair< TKey, TValue >* Map< TKey, TValue >::getEnd()
	{
		return m_pData + m_length;
	}

	template< class TKey, class TValue >
	const Pair< TKey, TValue >* Map< TKey, TValue >::getEnd() const
	{
		return m_pData + m_length;
	}

	template< class TKey, class TValue >
	ArrayView< Pair< TKey, TValue > > Map< TKey, TValue >::toArrayView() const
	{
		return ArrayView< PairType >( m_pData, m_length );
	}

	template< class TKey, class TValue >
	Map< TKey, TValue >& Map< TKey, TValue >::operator=( const Map& rhs )
	{
		clear();
		checkCapacity( rhs.m_length );
		m_length = rhs.m_length;
		for( uintreg i = 0u; i < m_length; ++i )
		{
			m_pData[ i ] = rhs.m_pData[ i ];
		}
		return *this;
	}

	template< class TKey, class TValue >
	TValue& Map< TKey, TValue >::operator[]( const TKey& key )
	{
		const uintreg index = findIndex( key );
		if( index == ( uintreg )-1 )
		{
			PairType* pPair = nullptr;
			findOrInsertPair( &pPair, key );
			pPair->value = TValue();

			return pPair->value;
		}

		return m_pData[ index ].value;
	}

	template< class TKey, class TValue >
	const TValue& Map< TKey, TValue >::operator[]( const TKey& key ) const
	{
		const uintreg index = findIndex( key );
		CONCT_ASSERT( index != ( uintreg )-1 );
		return m_pData[ index ].value;
	}

	template< class TKey, class TValue >
	void Map< TKey, TValue >::checkCapacity( uintreg capacity )
	{
		const uintreg nextCapacity = getNextPowerOfTwo( capacity );
		if( nextCapacity < m_capacity )
		{
			return;
		}

		PairType* pNewData = new PairType[ nextCapacity ];
		CONCT_ASSERT( pNewData != nullptr );

		for( uintreg i = 0u; i < m_length; ++i )
		{
			pNewData[ i ] = m_pData[ i ];
		}

		delete[] m_pData;
		m_pData = pNewData;
		m_capacity = nextCapacity;
	}

	template< class TKey, class TValue >
	uintreg Map< TKey, TValue >::findIndex( const TKey& key ) const
	{
		if( m_length == 0u )
		{
			return (uintreg)-1;
		}

		int imin = 0u;
		int imax = int( m_length );
		while( imax >= imin )
		{
			const int imid =  ( imax + imin ) / 2u;
			if( imid < 0 || imid >= ( int )m_length )
			{
				return ( uintreg )-1;
			}

			if( m_pData[ imid ].key == key )
			{
				return imid;
			}
			else if( m_pData[ imid ].key < key )
			{
				imin = imid + 1;
			}
			else
			{
				imax = imid - 1;
			}
		}

		return ( uintreg )-1;
	}

	template< class TKey, class TValue >
	bool Map< TKey, TValue >::findOrInsertPair( PairType** ppPair, const TKey& key )
	{
		uintreg pos = 0u;
		for( ; pos < m_length; ++pos )
		{
			PairType& pair = m_pData[ pos ];

			if( pair.key == key )
			{
				*ppPair = &pair;
				return false;
			}
			else if( pair.key > key )
			{
				break;
			}
		}

		checkCapacity( m_length + 1 );

		for( uintreg i = m_length; i > pos; --i )
		{
			m_pData[ i ] = m_pData[ i - 1 ];
		}

		PairType& pair = m_pData[ pos ];
		pair.key = key;
		m_length++;

		*ppPair = &pair;
		return true;
	}
}
