#pragma once

#include "conct_core.h"

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )

namespace conct
{
	class DynamicString;

	class Serial
	{
	public:

		bool						setup( const DynamicString& portName );
		bool						begin( int speed );

		void						flush();

		int							available();

		int							read();
		uintreg						readData( void* pBuffer, uintreg bufferLength );
		void						write( uint8 data );

	private:

		int							m_fileDescriptor;
	};
}

#endif