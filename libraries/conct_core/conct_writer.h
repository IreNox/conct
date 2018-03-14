#pragma once

#include "conct_core.h"

namespace conct
{
	class Writer
	{
	public:

					Writer();
					Writer( void* pData, uintreg size );

		void		set( void* pData, uintreg size );

		uintreg		getRemainingSize() const { return m_remainingSize; }
		bool		isEnd() const { return m_remainingSize == 0u; }

		template< class T >
		uintreg		writeStruct( const T& data );
		template< class T >
		uintreg		writeStruct( const T& data, uintreg remainingLength );
		uintreg		writeData( const void* pData, uintreg length );
		uintreg		writeData( const void* pData, uintreg length, uintreg remainingLength );

		bool		writeByte( uint8 data );

	private:

		uint8*		m_pData;
		uintreg		m_remainingSize;
	};
}

#include "conct_writer.inl"
