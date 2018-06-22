#pragma once

#include "conct_core.h"

#include <atomic>

namespace conct
{
	template< class T >
	struct AtomicTypeTraits
	{
		typedef T ValueType;

		static ValueType	toValueType( T value ) { return value; }
		static T			fromValueType( ValueType value ) { return value; }
	};

	template< >
	struct AtomicTypeTraits< bool >
	{
		typedef uint32 ValueType;

		static ValueType	toValueType( bool value ) { return value ? 1u : 0u; }
		static bool			fromValueType( ValueType value ) { return value != 0u ? true : false; }
	};

	template< class T >
	struct AtomicAccesorTraits
	{
	};

	template< >
	struct AtomicAccesorTraits< uint32 >
	{
		static uint32	loadRelaxed( const uint32* pValue );
		static uint32	loadAquire( const uint32* pValue );
		static uint32	loadOrdered( const uint32* pValue );

		static void		storeRelaxed( uint32* pValue, uint32 newValue );
		static void		storeRelease( uint32* pValue, uint32 newValue );
		static void		storeOrdered( uint32* pValue, uint32 newValue );

		static void		orRelaxed( uint32* pValue, uint32 mask );
		static void		orRelease( uint32* pValue, uint32 mask );
		static void		orOrdered( uint32* pValue, uint32 mask );

		static void		andRelaxed( uint32* pValue, uint32 mask );
		static void		andRelease( uint32* pValue, uint32 mask );
		static void		andOrdered( uint32* pValue, uint32 mask );
	};

	template< class T >
	class Atomic
	{
	public:

		typedef AtomicTypeTraits< T >				ValueTraits;
		typedef typename ValueTraits::ValueType		ValueType;
		typedef AtomicAccesorTraits< ValueType >	AccessorTraits;

					Atomic();

		T			getRelaxed() const;
		T			getAquire() const;
		T			getOrdered() const;

		void		setRelaxed( T value );
		void		setRelease( T value );
		void		setOrdered( T value );

		void		orRelaxed( T value );
		void		orRelease( T value );
		void		orOrdered( T value );

		void		andRelaxed( T value );
		void		andRelease( T value );
		void		andOrdered( T value );

	private:

		ValueType	m_value;
	};
}

#include "conct_atomic.inl"
