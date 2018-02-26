#pragma once

#include "conct_array.h"
#include "conct_core.h"

namespace conct
{
	class ReadStream
	{
	public:

		ReadStream( const uint8_t* pData, uint8_t size );

		template< class T >
		const T*	readStruct();
		uint8_t		readData( void* pTarget, uint8_t length );

	private:

		Array< uint8_t >	m_data;
		uint8_t				m_offset;
	};
}

#include "conct_read_stream.inl"
