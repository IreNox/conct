#pragma once

#include "conct_core.h"

#include <initializer_list>

namespace conct
{
	template< class T >
	class UnsortedSet
	{
	public:

						UnsortedSet();
						UnsortedSet( const UnsortedSet& rhs );
						UnsortedSet( const std::initializer_list< T >& initList );
						~UnsortedSet();

		bool			isEmpty() const;
		uintreg			getLength() const;
		uintreg			getCapacity() const;

		void			clear();
		void			reserve( uintreg size );

		bool			insert( const T& value );

		void			eraseSorted( const T& value );
		void			eraseSorted( const T* pValue );
		void			eraseSortedByIndex( uintreg index );
		void			eraseUnsorted( const T& value );
		void			eraseUnsorted( const T* pValue );
		void			eraseUnsortedByIndex( uintreg index );

		T*				getData();
		const T*		getData() const;

		T*				getBegin();
		const T*		getBegin() const;
		T*				getEnd();
		const T*		getEnd() const;

		T&				getFirst();
		const T&		getFirst() const;
		T&				getLast();
		const T&		getLast() const;

		UnsortedSet&	operator=( const UnsortedSet& rhs );

		T&				operator[]( uintreg index );
		const T&		operator[]( uintreg index ) const;

		T*				begin() { return getBegin(); }
		const T*		begin() const { return getBegin(); }
		T*				end() { return getEnd(); }
		const T*		end() const { return getEnd(); }

	private:

		T*				m_pData;
		uintreg			m_length;
		uintreg			m_capacity;

		void			checkCapacity( uintreg capacity );
	};
}

#include "conct_unsorted_set.inl"
