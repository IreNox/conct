#pragma once

#include "conct_core.h"

namespace conct
{
	template< class T >
	class Array
	{
	public:

						Array();
						Array( T* pData, uintreg count );

		void			set( T* pData, uintreg count );

		const uint8*	getData() const { return m_pData; }
		uintreg			getCount() const { return m_count; }

		const T&		operator[]( uintreg index ) const;

	private:

		T*		m_pData;
		uintreg	m_count;
	};
}

#include "conct_array.inl"