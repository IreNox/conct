#pragma once

#include "conct_functions.h"

namespace conct
{
	template< class TType, class TValueType >
	RelativeArray< TType, TValueType >::RelativeArray()
	{
		set( nullptr, 0u );
	}

	template< class TType, class TValueType >
	TType* RelativeArray< TType, TValueType >::get()
	{
		if( !isValid() )
		{
			return nullptr;
		}

		return addPointerCast< TType >( this, m_offset );
	}

	template< class TType, class TValueType >
	const TType* RelativeArray< TType, TValueType >::get() const
	{
		if( !isValid() )
		{
			return nullptr;
		}

		return addPointerCast< TType >( this, m_offset );
	}

	template< class TType, class TValueType >
	void RelativeArray< TType, TValueType >::set( TType* pData, TValueType size )
	{
		if( pData == nullptr )
		{
			CONCT_ASSERT( size == 0u );
			m_size		= 0u;
			m_offset	= 0u;
		}
		else
		{
			CONCT_ASSERT( ( uintptr )pData - ( uintptr )this < NumberLimits< TValueType >::max() );
			m_size = size;
			m_offset = TValueType( uintptr( pData ) - uintptr( this ) );
		}
	}

	template< class TType, class TValueType >
	bool RelativeArray< TType, TValueType >::isValid() const
	{
		return m_offset != 0u;
	}

	template< class TType, class TValueType >
	TValueType RelativeArray< TType, TValueType >::getSize() const
	{
		return m_size;
	}

	template< class TType, class TValueType >
	TValueType RelativeArray< TType, TValueType >::getOffset() const
	{
		return m_offset;
	}

	template< class TType, class TValueType >
	ArrayView< TType > RelativeArray< TType, TValueType >::toView() const
	{
		return ArrayView< TType >( get(), m_size );
	}

	template< class TType, class TValueType >
	RelativeArray< TType, TValueType >& RelativeArray< TType, TValueType >::operator=( const RelativeArray& rhs )
	{
		set( const_cast< TType* >( rhs.get() ), rhs.getSize() );
		return *this;
	}

	template< class TType, class TValueType >
	RelativeArray< TType, TValueType >& RelativeArray< TType, TValueType >::operator=( Array< TType > rhs )
	{
		set( rhs.getData(), TValueType( rhs.getLength() ) );
		return *this;
	}

	template< class TType, class TValueType >
	TType& RelativeArray< TType, TValueType >::operator[]( uintreg index )
	{
		CONCT_ASSERT( index < m_size );
		return *( get() + index );
	}

	template< class TType, class TValueType >
	const TType& RelativeArray< TType, TValueType >::operator[]( uintreg index ) const
	{
		CONCT_ASSERT( index < m_size );
		return *( get() + index );
	}
}
