#pragma once

#include "conct_core.h"

namespace conct
{
	template< class T >
	class ArrayView
	{
	public:

						ArrayView();
						ArrayView( const T* pData, muint count );

		void			set( const T* pData, muint count );

		const T*		getData() const { return m_pData; }
		muint			getCount() const { return m_count; }

		const T&		operator[]( muint index ) const;

	private:

		const T*		m_pData;
		muint			m_count;
	};
}

#include "conct_array_view.inl"