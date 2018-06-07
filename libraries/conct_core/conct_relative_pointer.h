#pragma once

#include "conct_core.h"

namespace conct
{
	template< class TType, class TValueType >
	class RelativePointer
	{
	public:

							RelativePointer() = default;
							RelativePointer( const RelativePointer& ) = delete;

		TType*				get();
		const TType*		get() const;
		void				set( TType* pData );

		bool				isValid() const;

		TValueType			getOffset() const;

		RelativePointer&	operator=( TType* pData );
							operator TType*() const;

	private:

		TValueType			m_offset;
	};

	template< class T >
	using RelativePointer8 = RelativePointer< T, uint8 >;

	template< class T >
	using RelativePointer16 = RelativePointer< T, uint16 >;

	template< class T >
	using RelativePointer32 = RelativePointer< T, uint32 >;

	template< class T >
	using RelativePointer64 = RelativePointer< T, uint64 >;
}

#include "conct_relative_pointer.inl"
