#pragma once

#include "conct_core.h"

namespace conct
{
	template< class TEnum, class TStorage >
	class FlagsBase
	{
	public:

					FlagsBase();
					FlagsBase( TEnum value );

		void		clear();

		bool		isEmpty() const;
		bool		isAnySet() const;
		bool		isSet( TEnum value ) const;

		void		set( TEnum value );
		void		unset( TEnum value );

		FlagsBase	operator~() const;
		FlagsBase	operator|( const FlagsBase& rhs ) const;
		FlagsBase	operator&( const FlagsBase& rhs ) const;

		FlagsBase&	operator|=( const FlagsBase& rhs );
		FlagsBase&	operator&=( const FlagsBase& rhs );

		bool		operator==( const FlagsBase& rhs ) const;
		bool		operator!=( const FlagsBase& rhs ) const;

	private:

		TStorage	m_flags;
	};

	template< class TEnum >
	using Flags8 = FlagsBase< TEnum, uint8 >;

	template< class TEnum >
	using Flags16 = FlagsBase< TEnum, uint16 >;

	template< class TEnum >
	using Flags32 = FlagsBase< TEnum, uint32 >;

	template< class TEnum >
	using Flags64 = FlagsBase< TEnum, uint64 >;
}

#include "conct_flags.inl"
