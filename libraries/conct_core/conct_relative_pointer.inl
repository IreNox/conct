#pragma once

namespace conct
{
	template< class TType, class TValueType >
	TType* RelativePointer< TType, TValueType >::get()
	{
		if( !isValid() )
		{
			return nullptr;
		}

		return addPointerCast< TType >( this, m_offset );
	}

	template< class TType, class TValueType >
	const TType* RelativePointer< TType, TValueType >::get() const
	{
		if( !isValid() )
		{
			return nullptr;
		}

		return addPointerCast< TType >( this, m_offset );
	}

	template< class TType, class TValueType >
	void RelativePointer< TType, TValueType >::set( TType* pData )
	{
		if( pData == nullptr )
		{
			m_offset = 0u;
			return;
		}

		CONCT_ASSERT( ( uintptr )pData - ( uintptr )this < NumberLimits< TValueType >::max() );
		m_offset = TValueType( ( uintptr )pData - ( uintptr )this );
	}

	template< class TType, class TValueType >
	bool RelativePointer< TType, TValueType >::isValid() const
	{
		return m_offset != 0u;
	}

	template< class TType, class TValueType >
	TValueType RelativePointer< TType, TValueType >::getOffset() const
	{
		return m_offset;
	}

	template< class TType, class TValueType >
	RelativePointer< TType, TValueType >& RelativePointer< TType, TValueType >::operator=( TType* pData )
	{
		set( pData );
		return *this;
	}

	template< class TType, class TValueType >
	RelativePointer< TType, TValueType >::operator TType*( ) const
	{
		return get();
	}
}
