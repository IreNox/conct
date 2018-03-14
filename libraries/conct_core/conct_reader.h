#pragma once

#include "conct_array.h"
#include "conct_core.h"

namespace conct
{
	class Reader
	{
	public:

						Reader();
						Reader( const void* pData, uintreg size );

		void			set( const void* pData, uintreg size );

		uintreg			getRemainingSize() const { return m_remainingSize; }
		bool			isEnd() const { return m_remainingSize == 0u; }

		template< class T >
		uintreg			readStruct( T& target );
		template< class T >
		uintreg			readStruct( T& Target, uintreg alreadyRead );
		uintreg			readData( void* pTarget, uintreg length );
		uintreg			readData( void* pTarget, uintreg length, uintreg alreadyRead );

		bool			readByte( uint8& target );
		uintreg			readShort( uint16& target );
		uintreg			readShort( uint16& target, uintreg alreadyRead );

	private:

		const uint8*	m_pData;
		uintreg			m_remainingSize;
	};
}

#include "conct_reader.inl"
