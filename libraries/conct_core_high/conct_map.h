#pragma once

#include "conct_array_view.h"
#include "conct_core.h"

#include <initializer_list>

namespace conct
{
	template< class TKey, class TValue >
	struct Pair
	{
		TKey	key;
		TValue	value;
	};

	template< class TKey, class TValue >
	class Map
	{
	public:

		typedef Pair< TKey, TValue > PairType;

								Map();
								Map( const Map& rhs );
								Map( const std::initializer_list< PairType >& initList );
								~Map();

		bool					isEmpty() const;
		uintreg					getLength() const;
		uintreg					getCapacity() const;

		void					clear();
		void					reserve( uintreg size );

		bool					find( TValue*& target, const TKey& key );
		bool					find( const TValue*& target, const TKey& key ) const;
		bool					findAndCopy( TValue& target, const TKey& key ) const;

		bool					insert( const TKey& key, const TValue& value );
		bool					erase( const TKey& key );

		PairType*				getData();
		const PairType*			getData() const;

		PairType*				getBegin();
		const PairType*			getBegin() const;
		PairType*				getEnd();
		const PairType*			getEnd() const;

		ArrayView< PairType >	toArrayView() const;

		Map&					operator=( const Map& rhs );

		TValue&					operator[]( const TKey& key );
		const TValue&			operator[]( const TKey& key ) const;

		PairType*				begin() { return getBegin(); }
		const PairType*			begin() const { return getBegin(); }
		PairType*				end() { return getEnd(); }
		const PairType*			end() const { return getEnd(); }

	private:

		PairType*				m_pData;
		uintreg					m_length;
		uintreg					m_capacity;

		void					checkCapacity( uintreg capacity );

		uintreg					findIndex( const TKey& key ) const;
		bool					findOrInsertPair( PairType** ppPair, const TKey& key );
	};
}

#include "conct_map.inl"
