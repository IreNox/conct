#pragma once

#include "conct_array_view.h"
#include "conct_core.h"

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

		bool			isEmpty() const;
		uintreg			getLength() const;
		uintreg			getCapacity() const;

		void			clear();
		void			reserve( uintreg size );
		void			setLength( uintreg size );

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

		T*				getData();
		const T*		getData() const;

		T*				getBegin();
		const T*		getBegin() const;
		T*				getEnd();
		const T*		getEnd() const;

		T&				getFront();
		const T&		getFront() const;
		T&				getBack();
		const T&		getBack() const;

		ArrayView< T >	toArrayView() const;

		Vector&			operator=( const Vector& rhs );

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

#include "conct_vector.inl"
