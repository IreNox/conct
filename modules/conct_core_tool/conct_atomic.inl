#pragma once

namespace conct
{
	template< class T >
	Atomic< T >::Atomic()
	{
		setOrdered( T() );
	}

	template< class T >
	T Atomic< T >::getRelaxed() const
	{
		return AccessorTraits::loadRelaxed( &m_value );
	}

	template< class T >
	T Atomic< T >::getAquire() const
	{
		return AccessorTraits::loadAquire( &m_value );
	}

	template< class T >
	T Atomic< T >::getOrdered() const
	{
		return AccessorTraits::loadOrdered( &m_value );
	}

	template< class T >
	void Atomic< T >::setRelaxed( T value )
	{
		AccessorTraits::storeRelaxed( &m_value, ValueTraits::toValueType( value ) );
	}

	template< class T >
	void Atomic< T >::setRelease( T value )
	{
		AccessorTraits::storeRelease( &m_value, ValueTraits::toValueType( value ) );
	}

	template< class T >
	void Atomic< T >::setOrdered( T value )
	{
		AccessorTraits::storeOrdered( &m_value, ValueTraits::toValueType( value ) );
	}

	template< class T >
	void Atomic< T >::orRelaxed( T value )
	{
		AccessorTraits::orRelaxed( &m_value, ValueTraits::toValueType( value ) );
	}

	template< class T >
	void Atomic< T >::orRelease( T value )
	{
		AccessorTraits::orRelease( &m_value, ValueTraits::toValueType( value ) );
	}

	template< class T >
	void Atomic< T >::orOrdered( T value )
	{
		AccessorTraits::orOrdered( &m_value, ValueTraits::toValueType( value ) );
	}

	template< class T >
	void Atomic< T >::andRelaxed( T value )
	{
		AccessorTraits::andRelaxed( &m_value, ValueTraits::toValueType( value ) );
	}

	template< class T >
	void Atomic< T >::andRelease( T value )
	{
		AccessorTraits::andRelease( &m_value, ValueTraits::toValueType( value ) );
	}

	template< class T >
	void Atomic< T >::andOrdered( T value )
	{
		AccessorTraits::andOrdered( &m_value, ValueTraits::toValueType( value ) );
	}
}