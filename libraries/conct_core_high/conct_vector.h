#pragma once

#include "conct_array.h"
#include "conct_functions.h"

#include <initializer_list>

namespace conct
{
	template< class T >
	class Vector : public Array< T >
	{
	public:

						Vector();
						Vector( const Vector& rhs );
						Vector( const std::initializer_list< T >& initList );
						~Vector();

		uintreg			getCapacity() const;

		void			clear();
		void			reserve( uintreg capacity );
		void			setLengthValue( uintreg size, T value );
		void			setLengthUninitialized( uintreg size );

		T&				insert( uintreg index );
		void			insert( const T& value, uintreg index );

		T&				pushBack();
		void			pushBack( const T& value );
		void			pushRange( const Vector< T >& vector );
		void			pushRange( const ArrayView< T >& arrayView );
		void			pushRange( const T* pData, uintreg length );

		void			popBack();

		void			eraseSorted( const T& value );
		void			eraseSorted( const T* pValue );
		void			eraseSortedByIndex( uintreg index );
		void			eraseUnsorted( const T& value );
		void			eraseUnsorted( const T* pValue );
		void			eraseUnsortedByIndex( uintreg index );

		Vector&			operator=( const Vector& rhs );

	private:

		uintreg			m_capacity;

		void			checkCapacity( uintreg capacity );
	};
}

#include "conct_vector.inl"
