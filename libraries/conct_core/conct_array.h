#pragma once

namespace conct
{
	template< class T >
	class Array
	{
	public:

						Array( T* pData, uint8_t count );

		uint8_t			getCount() const { return m_count; }

		T*				operator[]( uint8_t index );

	private:

		T*		m_pData;
		uint8	m_count;
	};
}

#include "conct_array.inl"