#pragma once

namespace conct
{
	template< class T >
	Vector< T >::Vector()
		: m_capacity( 0u )
	{
	}

	template< class T >
	Vector< T >::Vector( const Vector& rhs )
		: m_capacity( 0u )
	{
		*this = rhs;
	}

	template< class T >
	Vector< T >::Vector( const std::initializer_list< T >& initList )
		: m_capacity( 0u )
	{
		pushRange( initList.begin(), initList.size() );
	}

	template< class T >
	Vector< T >::~Vector()
	{
		delete[] this->m_pData;
	}

	template< class T >
	uintreg Vector< T >::getCapacity() const
	{
		return m_capacity;
	}

	template< class T >
	void Vector< T >::clear()
	{
		this->m_length = 0u;
	}

	template< class T >
	void Vector< T >::reserve( uintreg size )
	{
		checkCapacity( size );
	}

	template< class T >
	void Vector< T >::setLength( uintreg size )
	{
		checkCapacity( size );
		this->m_length = size;
	}

	template< class T >
	T& Vector< T >::insert( uintreg index )
	{
		CONCT_ASSERT( index <= this->m_length );
		checkCapacity( this->m_length + 1u );

		if( index < this->m_length )
		{
			for( uintreg i = this->m_length; i > index; --i )
			{
				this->m_pData[ i ] = this->m_pData[ i - 1u ];
			}
		}

		this->m_length++;

		return this->m_pData[ index ];
	}

	template< class T >
	void Vector< T >::insert( const T& value, uintreg index )
	{
		T& target = insert( index );
		target = value;
	}

	template< class T >
	T& Vector< T >::pushBack()
	{
		checkCapacity( this->m_length + 1u );
		return this->m_pData[ this->m_length++ ];
	}

	template< class T >
	void Vector< T >::pushBack( const T& value )
	{
		checkCapacity( this->m_length + 1u );
		this->m_pData[ this->m_length++ ] = value;
	}

	template< class T >
	void Vector< T >::pushRange( const Vector< T >& vector )
	{
		pushRange( vector.getData(), vector.getLength() );
	}

	template< class T >
	void Vector< T >::pushRange( const ArrayView< T >& arrayView )
	{
		pushRange( arrayView.getData(), arrayView.getLength() );
	}

	template< class T >
	void Vector< T >::pushRange( const T* pData, uintreg length )
	{
		checkCapacity( this->m_length + length );
		for( uintreg i = 0u; i < length; ++i )
		{
			this->m_pData[ this->m_length++ ] = pData[ i ];
		}
	}

	template< class T >
	void Vector< T >::popBack()
	{
		CONCT_ASSERT( this->m_length > 0u );
		this->m_length--;
	}

	template< class T >
	void Vector< T >::eraseSorted( const T& value )
	{
		eraseSorted( &value );
	}

	template< class T >
	void Vector< T >::eraseSorted( const T* pValue )
	{
		CONCT_ASSERT( pValue >= this->m_pData && pValue < this->m_pData + this->m_length );
		eraseSortedByIndex( pValue - this->m_pData );
	}

	template< class T >
	void Vector< T >::eraseSortedByIndex( uintreg index )
	{
		this->m_length--;
		for( uintreg i = index; i < this->m_length; ++i )
		{
			this->m_pData[ i ] = this->m_pData[ i + 1u ];
		}
	}

	template< class T >
	void Vector< T >::eraseUnsorted( const T& value )
	{
		eraseUnsorted( &value );
	}

	template< class T >
	void Vector< T >::eraseUnsorted( const T* pValue )
	{
		CONCT_ASSERT( pValue >= this->m_pData && pValue < this->m_pData + this->m_length );
		eraseUnsortedByIndex( pValue - this->m_pData );
	}

	template< class T >
	void Vector< T >::eraseUnsortedByIndex( uintreg index )
	{
		this->m_length--;
		if( index != this->m_length )
		{
			this->m_pData[ index ] = this->m_pData[ this->m_length ];
		}
	}

	template< class T >
	Vector< T >& Vector< T >::operator=( const Vector& rhs )
	{
		clear();
		pushRange( rhs.getData(), rhs.getLength() );
		return *this;
	}

	template< class T >
	void Vector< T >::checkCapacity( uintreg capacity )
	{
		const uintreg nextCapacity = getNextPowerOfTwo( capacity );
		if( nextCapacity <= m_capacity )
		{
			return;
		}

		T* pNewData = new T[ nextCapacity ];
		CONCT_ASSERT( pNewData != nullptr );

		for( uintreg i = 0u; i < this->m_length; ++i )
		{
			pNewData[ i ] = this->m_pData[ i ];
		}

		delete[] this->m_pData;
		this->m_pData = pNewData;
		m_capacity = nextCapacity;
	}
}
