#pragma once

#include "conct_core.h"

#include <initializer_list>

namespace conct
{
	template< class T >
	class ArrayView
	{
	public:

							ArrayView();
							ArrayView( const T* pData, uintreg length );
		constexpr			ArrayView( const std::initializer_list< T >& initList );

		constexpr void		set( const T* pData, uintreg length );

		constexpr bool		isSet() const { return m_pData != nullptr; }
		constexpr uintreg	getLength() const { return m_length; }


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