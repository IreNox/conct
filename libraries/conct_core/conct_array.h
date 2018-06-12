#pragma once

#include "conct_core.h"

namespace conct
{
	template< class T >
	class Array
	{
	public:

					Array();
					Array( T* pData, uintreg length );

		void		set( T* pData, uintreg length );
		void		clear();

		bool		isSet() const { return m_pData != nullptr; }
		uintreg		getLength() const { return m_length; }

		T*			getData() { return m_pData; }
		const T*	getData() const { return m_pData; }

		T&			operator[]( uintreg index );
		const T&	operator[]( uintreg index ) const;

		T*			begin() { return m_pData; }
		T*			end() { return m_pData + m_length; }

	private:

		T*			m_pData;
		uintreg		m_length;
	};
}

#include "conct_array.inl"