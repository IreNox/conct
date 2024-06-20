#pragma once

#include "conct_core.h"

namespace conct
{
	namespace memory
	{
		int						compare( const void* pData1, const void* pData2, uintreg sizeInBytes );
		void					copy( void* pTargetData, const void* pSourceData, uintreg sizeInBytes );
		void					copyOverlapping( void* pTargetData, const void* pSourceData, uintreg sizeInBytes );

		CONCT_FORCE_INLINE void	set8( void* pTargetData, uintreg count, uint8 value );
		CONCT_FORCE_INLINE void	set16( void* pTargetData, uintreg count, uint16 value );
		CONCT_FORCE_INLINE void	set32( void* pTargetData, uintreg count, uint32 value );
		CONCT_FORCE_INLINE void	set64( void* pTargetData, uintreg count, uint64 value );
		CONCT_FORCE_INLINE void	zero( void* pTargetData, uintreg sizeInBytes );

		template< class T >
		CONCT_FORCE_INLINE void	zero( T& pTargetData );
	}
}

#include "conct_memory.inl"
