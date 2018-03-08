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
		muint		writeStruct( const T& data );
		template< class T >
		muint		writeStruct( const T& data, muint remainingLength );
		muint		writeData( const void* pData, muint length );
		muint		writeData( const void* pData, muint length, muint remainingLength );

		bool		writeByte( uint8_t data );

	private:

		uint8_t*	m_pData;
		muint		m_remainingSize;
	};
}

#include "conct_writer.inl"
