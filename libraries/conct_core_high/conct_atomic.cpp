#include "conct_atomic.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <xatomic.h>
#endif

namespace conct
{
	/*static*/ uint32 AtomicAccesorTraits< uint32 >::loadRelaxed( const uint32* pValue )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		return std::_Load_relaxed_4( (volatile std::_Uint4_t*)pValue );
#endif
	}

	/*static*/ uint32 AtomicAccesorTraits< uint32 >::loadAquire( const uint32* pValue )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		return std::_Load_acquire_4( ( volatile std::_Uint4_t* )pValue );
#endif
	}

	/*static*/ uint32 AtomicAccesorTraits< uint32 >::loadOrdered( const uint32* pValue )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		return std::_Load_seq_cst_4( ( volatile std::_Uint4_t* )pValue );
#endif
	}

	/*static*/ void AtomicAccesorTraits< uint32 >::storeRelaxed( uint32* pValue, uint32 newValue )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Store_relaxed_4( ( volatile std::_Uint4_t* )pValue, newValue );
#endif
	}

	/*static*/ void AtomicAccesorTraits< uint32 >::storeRelease( uint32* pValue, uint32 newValue )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Store_release_4( ( volatile std::_Uint4_t* )pValue, newValue );
#endif
	}

	/*static*/ void AtomicAccesorTraits< uint32 >::storeOrdered( uint32* pValue, uint32 newValue )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Store_seq_cst_4( ( volatile std::_Uint4_t* )pValue, newValue );
#endif
	}

	/*static*/ void AtomicAccesorTraits< uint32 >::orRelaxed( uint32* pValue, uint32 mask )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Fetch_or_relaxed_4( ( volatile std::_Uint4_t* )pValue, mask );
#endif
	}

	/*static*/ void AtomicAccesorTraits< uint32 >::orRelease( uint32* pValue, uint32 mask )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Fetch_or_release_4( ( volatile std::_Uint4_t* )pValue, mask );
#endif
	}

	/*static*/ void AtomicAccesorTraits< uint32 >::orOrdered( uint32* pValue, uint32 mask )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Fetch_or_seq_cst_4( ( volatile std::_Uint4_t* )pValue, mask );
#endif
	}

	/*static*/ void AtomicAccesorTraits< uint32 >::andRelaxed( uint32* pValue, uint32 mask )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Fetch_and_relaxed_4( ( volatile std::_Uint4_t* )pValue, mask );
#endif
	}

	/*static*/ void AtomicAccesorTraits< uint32 >::andRelease( uint32* pValue, uint32 mask )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Fetch_and_release_4( ( volatile std::_Uint4_t* )pValue, mask );
#endif
	}

	/*static*/ void AtomicAccesorTraits< uint32 >::andOrdered( uint32* pValue, uint32 mask )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		std::_Fetch_and_seq_cst_4( ( volatile std::_Uint4_t* )pValue, mask );
#endif
	}
}