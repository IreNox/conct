#pragma once

#include "conct_result.h"
#include "conct_value.h"

#include <tiki/tiki_array_view.h>

namespace conct
{
	class DataBuilder;

	class ValueBuilder
	{
	public:

		using ByteView = ArrayView< const byte >;

							ValueBuilder( DataBuilder* pBuilder, Value* pValue );

		const Value*		getValue() const { return m_pValue; }
		uintreg				getValueSize() const;
		ByteView			toArrayView() const;

		ResultId			setVoid();
		ResultId			setBoolean( bool value );
		ResultId			setInteger( sint32 value );
		ResultId			setUnsigned( uint32 value );
		ResultId			setString( const char* pValue );
		ResultId			setPercentValue( PercentValue value );
		ResultId			setDeviceId( DeviceId value );
		ResultId			setInstanceId( InstanceId value );
		ResultId			setTypeCrc( TypeCrc value );

		ResultId			setStruct( const void* pData, uintreg dataSize, TypeCrc type );
		template< class T >
		ResultId			setStruct( const T& value );

		ResultId			setArray( const void* pData, uintreg elementSize, uintreg length, TypeCrc type );
		template< class T >
		ResultId			setArray( const ArrayView< const T >& value );

		template< class T >
		ResultId			setValue( const T* pValue );

	private:

		DataBuilder*		m_pBuilder;
		Value*				m_pValue;
	};
}

#include "conct_value_builder.inl"
