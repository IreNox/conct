#pragma once

#include "conct_core.h"

namespace conct
{
	template< class T >
	class ArrayView
	{
	public:

							ArrayView();
							ArrayView( const T* pData, uintreg length );

		void				set( const T* pData, uintreg length );

		bool				isSet() const { return m_pData != nullptr; }
		uintreg				getLength() const { return m_length; }


		const T*			getData() const { return m_pData; }

		const T*			getBegin() const;
		const T*			getEnd() const;

		const T&			getFront() const;
		const T&			getBack() const;

		const T&			operator[]( uintreg index ) const;

		const T*			begin() const { return getBegin(); }
		const T*			end() const { return getEnd(); }

	private:

		const T*			m_pData;
		uintreg				m_length;
	};
}

#include "conct_array_view.inl"