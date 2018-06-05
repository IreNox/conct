#pragma once

#include <initializer_list>

namespace conct
{
	template< class T >
	class Vector
	{
	public:

					Vector();
					Vector( const std::initializer_list< T >& initList );
					~Vector();

		bool		isEmpty() const;
		uintreg		getSize() const;
		uintreg		getCapacity() const;

		void		reserve( uintreg size );

		T&			pushBack();
		void		pushBack( const T& value );

		void		popBack();

		void		eraseSorted( const T& value );
		void		eraseSorted( const T* pValue );
		void		eraseSortedByIndex( uintreg index );
		void		eraseUnsorted( const T& value );
		void		eraseUnsorted( const T* pValue );
		void		eraseUnsortedByIndex( uintreg index );

		T&			operator[]( uintreg index );
		const T&	operator[]( uintreg index ) const;

	private:

		T*			m_pData;
		uintreg		m_size;
		uintreg		m_capacity;

		void		checkCapacity( uintreg size );
	};
}

#include "conct_vector.inl"
