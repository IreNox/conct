#pragma once

#include "conct_core.h"

namespace conct
{
	template< class T >
	class Array
	{
	public:

						Array();
						Array( T* pData, uint8_t count );

		void			set( T* pData, uint8_t count );

		const uint8_t*	getData() const { return m_pData; }
		uint8_t			getCount() const { return m_count; }

		const T&		operator[]( uint8_t index ) const;

	private:

		T*		m_pData;
		uint8_t	m_count;
	};
}

#include "conct_array.inl"