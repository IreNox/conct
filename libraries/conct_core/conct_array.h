#pragma once

#include "conct_array_view.h"

namespace conct
{
	template< class T >
	class Array
	{
	public:

						Array();
						Array( T* pData, uintreg length );

		bool			isSet() const		{ return m_pData != nullptr; }
		bool			isEmpty() const		{ return m_length == 0u; }
		bool			hasElements() const	{ return m_length != 0u; }
		uintreg			getLength() const	{ return m_length; }

		T*				getData()			{ return m_pData; }
		const T*		getData() const		{ return m_pData; }

		T*				getBegin()			{ return m_pData; }
		const T*		getBegin() const	{ return m_pData; }
		T*				getEnd()			{ return m_pData + m_length; }
		const T*		getEnd() const		{ return m_pData + m_length; }

		T&				getFront();
		const T&		getFront() const;
		T&				getBack();
		const T&		getBack() const;

		T&				getElement( uintreg index );
		const T&		getElement( uintreg index ) const;
		T&				getReverseElement( uintreg index );
		const T&		getReverseElement( uintreg index ) const;

		ArrayView< T >	toView() const;

		T&				operator[]( uintreg index );
		const T&		operator[]( uintreg index ) const;

	protected:

		T*				m_pData;
		uintreg			m_length;
	};

	template< class T > T* begin( Array< T >& arr ) { return arr.getBegin(); }
	template< class T > T* end( Array< T >& arr ) { return arr.getEnd(); }
	template< class T > const T* begin( const Array< T >& arr ) { return arr.getBegin(); }
	template< class T > const T* end( const Array< T >& arr ) { return arr.getEnd(); }
}

#include "conct_array.inl"