#pragma once

#include "conct_array.h"
#include "conct_core.h"

namespace conct
{
	class Reader
	{
	public:

					Reader();
					Reader( const void* pData, muint size );

		void		set( const void* pData, muint size );

		muint		getRemainingSize() const { return m_remainingSize; }
		bool		isEnd() const { return m_remainingSize == 0u; }

		template< class T >
		muint		readStruct( T& target );
		template< class T >
		muint		readStruct( T& Target, muint alreadyRead );
		muint		readData( void* pTarget, muint length );
		muint		readData( void* pTarget, muint length, muint alreadyRead );

		bool		readByte( uint8_t& target );
		muint		readShort( uint16_t& target );
		muint		readShort( uint16_t& target, muint alreadyRead );

	private:

		const uint8_t*	m_pData;
		muint			m_remainingSize;
	};
}

#include "conct_reader.inl"
