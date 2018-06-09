#pragma once

#include <initializer_list>

namespace conct
{
	template< class T >
	class Vector
	{
	public:

					Vector();
					Vector( const Vector& rhs );
					Vector( const std::initializer_list< T >& initList );
					~Vector();

		bool		isEmpty() const;
		uintreg		getCount() const;
		uintreg		getCapacity() const;

		void		clear();
		void		reserve( uintreg size );

		T&			pushBack();
		void		pushBack( const T& value );
		void		pushRange( const T* pData, uintreg count );

		void		popBack();

		void		eraseSorted( const T& value );
		void		eraseSorted( const T* pValue );
		void		eraseSortedByIndex( uintreg index );
		void		eraseUnsorted( const T& value );
		void		eraseUnsorted( const T* pValue );
		void		eraseUnsortedByIndex( uintreg index );

		T*			getData();
		const T*	getData() const;

		Vector&		operator=( const Vector& rhs );

		T&			operator[]( uintreg index );
		const T&	operator[]( uintreg index ) const;

	private:

		T*			m_pData;
		uintreg		m_count;
		uintreg		m_capacity;

		void		checkCapacity( uintreg size );
	};
}

#include "conct_vector.inl"
