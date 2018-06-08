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
		void			clear();

		bool			isSet() const;

		T*				getData() { return m_pData; }
		const T*		getData() const { return m_pData; }

		uintreg			getCount() const { return m_count; }

		T*				begin() { return m_pData; }
		T*				end() { return m_pData + m_count; }

		T&				operator[]( uintreg index );
		const T&		operator[]( uintreg index ) const;

	private:

		T*		m_pData;
		uintreg	m_count;
	};
}

#include "conct_array.inl"