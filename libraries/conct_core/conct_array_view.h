#pragma once

#include "conct_core.h"

namespace conct
{
	template< class T >
	class ArrayView
	{
	public:

						ArrayView();
						ArrayView( const T* pData, uintreg count );

		void			set( const T* pData, uintreg count );

		const T*		getData() const { return m_pData; }
		uintreg			getCount() const { return m_count; }

		const T&		operator[]( uintreg index ) const;

	private:

		const T*		m_pData;
		uintreg			m_count;
	};
}

#include "conct_array_view.inl"