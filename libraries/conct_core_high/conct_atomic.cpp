#include "conct_atomic.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <xatomic.h>
#endif

namespace conct
{
	uint32_atomic AtomicAccesorTraits< uint32_atomic >::loadRelaxed( const uint32_atomic* pValue )
	{
#if CONCT_ENABLED( CONCT_ATOMIC_STD )
		return pValue->load( std::memory_order_relaxed );
#elif CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		return _InterlockedOr( (volatile long*)pValue, 0 );
#elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
		return __atomic_load_n( pValue, __ATOMIC_RELAXED );
#endif
	}

	uint32_atomic AtomicAccesorTraits< uint32_atomic >::loadAquire( const uint32_atomic* pValue )
	{
#if CONCT_ENABLED( CONCT_ATOMIC_STD )
		return pValue->load( std::memory_order_acquire );
#elif CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		return std::_Load_acquire_4( ( volatile std::_Uint4_t* )pValue );
#elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
		return __atomic_load_n( pValue, __ATOMIC_ACQUIRE );
#endif
	}

	uint32_atomic AtomicAccesorTraits< uint32_atomic >::loadOrdered( const uint32_atomic* pValue )
	{
#if CONCT_ENABLED( CONCT_ATOMIC_STD )
		return pValue->load( std::memory_order_seq_cst );
#elif CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		return std::_Load_seq_cst_4( ( volatile std::_Uint4_t* )pValue );
#elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
		return __atomic_load_n( pValue, __ATOMIC_SEQ_CST );
#endif
	}

	void AtomicAccesorTraits< uint32_atomic >::storeRelaxed( uint32_atomic* pValue, uint32 newValue )
	{
#if CONCT_ENABLED( CONCT_ATOMIC_STD )
		pValue->store( newValue, std::memory_order_relaxed );
#elif CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Store_relaxed_4( ( volatile std::_Uint4_t* )pValue, newValue );
#elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
		__atomic_store_n( pValue, newValue, __ATOMIC_RELAXED );
#endif
	}

	void AtomicAccesorTraits< uint32_atomic >::storeRelease( uint32_atomic* pValue, uint32 newValue )
	{
#if CONCT_ENABLED( CONCT_ATOMIC_STD )
		pValue->store( newValue, std::memory_order_release );
#elif CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Store_release_4( ( volatile std::_Uint4_t* )pValue, newValue );
#elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
		__atomic_store_n( pValue, newValue, __ATOMIC_RELEASE );
#endif
	}

	void AtomicAccesorTraits< uint32_atomic >::storeOrdered( uint32_atomic* pValue, uint32 newValue )
	{
#if CONCT_ENABLED( CONCT_ATOMIC_STD )
		pValue->store( newValue, std::memory_order_seq_cst );
#elif CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Store_seq_cst_4( ( volatile std::_Uint4_t* )pValue, newValue );
#elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
		__atomic_store_n( pValue, newValue, __ATOMIC_SEQ_CST );
#endif
	}

	void AtomicAccesorTraits< uint32_atomic >::orRelaxed( uint32_atomic* pValue, uint32 mask )
	{
#if CONCT_ENABLED( CONCT_ATOMIC_STD )
		pValue->fetch_or( mask, std::memory_order_relaxed );
#elif CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Fetch_or_relaxed_4( ( volatile std::_Uint4_t* )pValue, mask );
#elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
		__atomic_fetch_or( pValue, mask, __ATOMIC_RELAXED );
#endif
	}

	void AtomicAccesorTraits< uint32_atomic >::orRelease( uint32_atomic* pValue, uint32 mask )
	{
#if CONCT_ENABLED( CONCT_ATOMIC_STD )
		pValue->fetch_or( mask, std::memory_order_release );
#elif CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Fetch_or_release_4( ( volatile std::_Uint4_t* )pValue, mask );
#elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
		__atomic_fetch_or( pValue, mask, __ATOMIC_RELEASE );
#endif
	}

	void AtomicAccesorTraits< uint32_atomic >::orOrdered( uint32_atomic* pValue, uint32 mask )
	{
#if CONCT_ENABLED( CONCT_ATOMIC_STD )
		pValue->fetch_or( mask, std::memory_order_seq_cst );
#elif CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Fetch_or_seq_cst_4( ( volatile std::_Uint4_t* )pValue, mask );
#elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
		__atomic_fetch_or( pValue, mask, __ATOMIC_SEQ_CST );
#endif
	}

	void AtomicAccesorTraits< uint32_atomic >::andRelaxed( uint32_atomic* pValue, uint32 mask )
	{
#if CONCT_ENABLED( CONCT_ATOMIC_STD )
		pValue->fetch_and( mask, std::memory_order_relaxed );
#elif CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Fetch_and_relaxed_4( ( volatile std::_Uint4_t* )pValue, mask );
#elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
		__atomic_fetch_and( pValue, mask, __ATOMIC_RELAXED );
#endif
	}

	void AtomicAccesorTraits< uint32_atomic >::andRelease( uint32_atomic* pValue, uint32 mask )
	{
#if CONCT_ENABLED( CONCT_ATOMIC_STD )
		pValue->fetch_and( mask, std::memory_order_release );
#elif CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Fetch_and_release_4( ( volatile std::_Uint4_t* )pValue, mask );
#elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
		__atomic_fetch_and( pValue, mask, __ATOMIC_RELEASE );
#endif
	}

	void AtomicAccesorTraits< uint32_atomic >::andOrdered( uint32_atomic* pValue, uint32 mask )
	{
#if CONCT_ENABLED( CONCT_ATOMIC_STD )
		pValue->fetch_and( mask, std::memory_order_seq_cst );
#elif CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		_InterlockedAnd( (volatile long*)pValue, mask );
		std::_Fetch_and_seq_cst_4( ( volatile std::_Uint4_t* )pValue, mask );
#elif CONCT_ENABLED( CONCT_COMPILER_GCC ) || CONCT_ENABLED( CONCT_COMPILER_CLANG )
		__atomic_fetch_and( pValue, mask, __ATOMIC_SEQ_CST );
#endif
	}
}