#pragma once

namespace conct
{
	template< class T, uintreg TSize >
	StaticArray< T, TSize >::StaticArray()
	{
		CONCT_STATIC_ASSERT( TSize > 0u );
	}

	template< class T, uintreg TSize >
	uintreg StaticArray<T, TSize>::getLength() const
	{
		return TSize;
	}

	template< class T, uintreg TSize >
	T* StaticArray<T, TSize>::getData()
	{
		return m_aData;
	}

	template< class T, uintreg TSize >
	const T* StaticArray<T, TSize>::getData() const
	{
		return m_aData;
	}

	template< class T, uintreg TSize >
	T* StaticArray<T, TSize>::getBegin()
	{
		return m_aData;
	}

	template< class T, uintreg TSize >
	const T* StaticArray<T, TSize>::getBegin() const
	{
		return m_aData;
	}

	template< class T, uintreg TSize >
	T* StaticArray<T, TSize>::getEnd()
	{
		return m_aData + TSize;
	}

	template< class T, uintreg TSize >
	const T* StaticArray<T, TSize>::getEnd() const
	{
		return m_aData + TSize;
	}

	template< class T, uintreg TSize >
	T& StaticArray<T, TSize>::getFront()
	{
		return m_aData[ 0u ];
	}

	template< class T, uintreg TSize >
	const T& StaticArray<T, TSize>::getFront() const
	{
		return m_aData[ 0u ];
	}

	template< class T, uintreg TSize >
	T& StaticArray<T, TSize>::getBack()
	{
		return m_aData[TSize - 1u];
	}

	template< class T, uintreg TSize >
	const T& StaticArray<T, TSize>::getBack() const
	{
		return m_aData[ TSize - 1u ];
	}

	template< class T, uintreg TSize >
	ArrayView< T > StaticArray<T, TSize>::toArrayView() const
	{
		return ArrayView< T >( m_aData, TSize );
	}

	template< class T, uintreg TSize >
	T& StaticArray< T, TSize >::operator[]( uintreg index )
	{
		CONCT_ASSERT( index < TSize);
		return m_aData[ index ];
	}

	template< class T, uintreg TSize >
	const T& StaticArray< T, TSize >::operator[]( uintreg index ) const
	{
		CONCT_ASSERT( index < TSize);
		return m_aData[ index ];
	}
}
