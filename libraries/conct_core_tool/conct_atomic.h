#pragma once

#include "conct_core.h"

#include <atomic>

#if defined( _MSC_VER ) && _MSC_VER >= 1920
#	define CONCT_ATOMIC_STD CONCT_ON
#else
#	define CONCT_ATOMIC_STD CONCT_OFF
#endif

namespace conct
{
#if CONCT_ENABLED( CONCT_ATOMIC_STD )
	using uint32_atomic = std::atomic< uint32 >;
#elif CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
	using uint32_atomic = volatile uint32;
#elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
	using uint32_atomic = uint32;
#else
#	error Platform not supported
#endif

	template< class T >
	struct AtomicTypeTraits
	{
		typedef uint32_atomic ValueType;

		static ValueType	toValueType( T value ) { return value; }
		static T			fromValueType( ValueType value ) { return value; }
	};

	template< >
	struct AtomicTypeTraits< bool >
	{
		typedef uint32_atomic ValueType;

		static ValueType	toValueType( bool value ) { return value ? 1u : 0u; }
		static bool			fromValueType( ValueType value ) { return value != 0u ? true : false; }
	};

	template< class T >
	struct AtomicAccesorTraits
	{
	};

	template< >
	struct AtomicAccesorTraits< uint32_atomic >
	{
		static uint32_atomic	loadRelaxed( const uint32_atomic* pValue );
		static uint32_atomic	loadAquire( const uint32_atomic* pValue );
		static uint32_atomic	loadOrdered( const uint32_atomic* pValue );

		static void		storeRelaxed( uint32_atomic* pValue, uint32 newValue );
		static void		storeRelease( uint32_atomic* pValue, uint32 newValue );
		static void		storeOrdered( uint32_atomic* pValue, uint32 newValue );

		static void		orRelaxed( uint32_atomic* pValue, uint32 mask );
		static void		orRelease( uint32_atomic* pValue, uint32 mask );
		static void		orOrdered( uint32_atomic* pValue, uint32 mask );

		static void		andRelaxed( uint32_atomic* pValue, uint32 mask );
		static void		andRelease( uint32_atomic* pValue, uint32 mask );
		static void		andOrdered( uint32_atomic* pValue, uint32 mask );
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
