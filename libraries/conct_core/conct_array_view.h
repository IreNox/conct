#pragma once

#include "conct_core.h"

namespace conct
{
	template< class T >
	class ArrayView
	{
	public:

						ArrayView();
						ArrayView( const T* pData, uint8_t count );

		void			set( const T* pData, uint8_t count );

		const T*		getData() const { return m_pData; }
		uint8_t			getCount() const { return m_count; }

		const T&		operator[]( uint8_t index ) const;

	private:

		const T*		m_pData;
		uint8_t			m_count;
	};
}

#include "conct_array_view.inl"