#pragma once

#include "conct_core.h"

namespace conct
{
	class Writer
	{
	public:

					Writer();
					Writer( void* pData, uint8_t size );

		void		set( void* pData, uint8_t size );

		uint8_t		getRemainingSize() const { return m_remainingSize; }
		bool		isEnd() const { return m_remainingSize == 0u; }

		template< class T >
		bool		writeStruct( const T& data );
		bool		writeData( const uint8_t* pData, uint8_t length );

		bool		writeByte( uint8_t data );

	private:

		uint8_t*	m_pData;
		uint8_t		m_remainingSize;
	};
}

#include "conct_writer.inl"
