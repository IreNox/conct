#pragma once

#include "conct_array.h"
#include "conct_core.h"

namespace conct
{
	class Reader
	{
	public:

					Reader();
					Reader( const void* pData, uint8_t size );

		void		set( const void* pData, uint8_t size );

		uint8_t		getRemainingSize() const { return m_remainingSize; }
		bool		isEnd() const { return m_remainingSize == 0u; }

		template< class T >
		const T*	readStruct();
		const void*	readData( uint8_t length );
		bool		readData( uint8_t* pTarget, uint8_t length );

		bool		readByte( uint8_t& target );

	private:

		const uint8_t*	m_pData;
		uint8_t			m_remainingSize;
	};
}

#include "conct_reader.inl"
