#pragma once

namespace conct
{
	template< class TEnum, class TStorage >
	FlagsBase<TEnum, TStorage>::FlagsBase()
	{
		clear();
	}

	template< class TEnum, class TStorage >
	FlagsBase<TEnum, TStorage>::FlagsBase( TEnum value )
	{
		m_flags = ( TStorage )value;
	}

	template< class TEnum, class TStorage >
	void FlagsBase<TEnum, TStorage>::clear()
	{
		m_flags = 0;
	}

	template< class TEnum, class TStorage >
	bool FlagsBase<TEnum, TStorage>::isEmpty() const
	{
		return m_flags == 0;
	}

	template< class TEnum, class TStorage >
	bool FlagsBase<TEnum, TStorage>::isAnySet() const
	{
		return m_flags != 0;
	}

	template< class TEnum, class TStorage >
	bool FlagsBase<TEnum, TStorage>::isSet( TEnum value ) const
	{
		return ( m_flags & value ) == value;
	}

	template< class TEnum, class TStorage >
	void FlagsBase<TEnum, TStorage>::set( TEnum value )
	{
		m_flags |= ( TStorage )value;
	}

	template< class TEnum, class TStorage >
	void FlagsBase<TEnum, TStorage>::unset( TEnum value )
	{
		m_flags &= ~( TStorage )value;
	}

	template< class TEnum, class TStorage >
	FlagsBase< TEnum, TStorage > FlagsBase<TEnum, TStorage>::operator~() const
	{
		FlagsBase result;
		result.m_flags = ~m_flags;
		return result;
	}

	template< class TEnum, class TStorage >
	FlagsBase< TEnum, TStorage > FlagsBase<TEnum, TStorage>::operator|( const FlagsBase& rhs ) const
	{
		FlagsBase result;
		result.m_flags = ( m_flags | rhs.m_flags );
		return result;
	}

	template< class TEnum, class TStorage >
	FlagsBase< TEnum, TStorage > FlagsBase<TEnum, TStorage>::operator&( const FlagsBase& rhs ) const
	{
		FlagsBase result;
		result.m_flags = ( m_flags & rhs.m_flags );
		return result;
	}

	template< class TEnum, class TStorage >
	FlagsBase< TEnum, TStorage >& FlagsBase<TEnum, TStorage>::operator|=( const FlagsBase& rhs )
	{
		m_flags |= rhs.m_flags;
		return *this;
	}

	template< class TEnum, class TStorage >
	FlagsBase< TEnum, TStorage >& FlagsBase<TEnum, TStorage>::operator&=( const FlagsBase& rhs )
	{
		m_flags &= rhs.m_flags;
		return *this;
	}

	template< class TEnum, class TStorage >
	bool FlagsBase<TEnum, TStorage>::operator==( const FlagsBase& rhs ) const
	{
		return m_flags == rhs.m_flags;
	}

	template< class TEnum, class TStorage >
	bool FlagsBase<TEnum, TStorage>::operator!=( const FlagsBase& rhs ) const
	{
		return m_flags != rhs.m_flags;
	}
}
