#pragma once

#include "conct_array_view.h"
#include "conct_core.h"
#include "conct_pair.h"

#include <initializer_list>

namespace conct
{
	template< class TKey, class TValue >
	class Map
	{
	public:

		typedef Pair< TKey, TValue > PairType;

		struct InsertResult
		{
			bool				isNew;
			TValue*				pValue;
		};

								Map();
								Map( const Map& rhs );
								Map( const std::initializer_list< PairType >& initList );
								~Map();

		bool					isEmpty() const;
		uintreg					getLength() const;
		uintreg					getCapacity() const;

		void					clear();
		void					reserve( uintreg size );

		bool					hasKey( const TKey& key ) const;

		TValue*					find( const TKey& key );
		const TValue*			find( const TKey& key ) const;
		bool					findAndCopy( TValue& target, const TKey& key ) const;

		InsertResult			insertKey( const TKey& key );
		void					insert( const TKey& key, const TValue& value );
		bool					remove( const TKey& key );

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

		uintreg					findIndex( const TKey& key ) const;
		uintreg					findPositionIndex( const TKey& key ) const;

		uintreg					getNextCapacity( uintreg neededCapacity );
		void					checkCapacity( uintreg neededCapacity );
	};
}

#include "conct_map.inl"
