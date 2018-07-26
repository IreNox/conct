#pragma once

#include "conct_core.h"

#if CONCT_ENABLED( CONCT_ENVIRONMENT_TESTS )

namespace conct
{
	class SerialTestContext;

	class Serial
	{
	public:

		bool						begin( int speed );

		void						flush();

		int							available();

		int							read();
		uintreg						readData( void* pBuffer, uintreg bufferLength );
		void						write( uint8 data );

	private:

		static SerialTestContext*	s_pContext;
	};
}

#endif