#pragma once

#include "conct_result.h"
#include "conct_value.h"

#include <tiki/tiki_array_view.h>

namespace conct
{
	class DataBuilder;

	class ValueWriter
	{
	public:

		using ByteView = ConstArrayView< byte >;

							ValueWriter();

		ByteView			getData() const { return m_data; }

		template< typename T >
		T*					writeStruct();

		void				writeValue( ValueSerializeData& targetData, const Value& sourceValue );

	private:

		using ByteArray = DynamicArray< byte >;

		ByteArray			m_data;

		void*				writeBytes( uintsize size );
	};
}

#include "conct_value_writer.inl"
