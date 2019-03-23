#pragma once

#include "conct_dynamic_string.h"
#include "conct_value.h"
#include "conct_vector.h"
#include "conct_array_view.h"

namespace conct
{
	class ValueHigh
	{
	public:

						ValueHigh();
						ValueHigh( const Value& value );

		ValueType		getType() const;

		bool			getBoolean() const;
		sint32			getInteger() const;
		uint32			getUnsigned() const;
		const char*		getString() const;
		PercentValue	getPercentValue() const;
		DeviceId		getDeviceId() const;
		InstanceId		getInstanceId() const;
		TypeCrc			getTypeCrc() const;

		template< class T >
		const T&		getStruct() const;
		const void*		getStructData() const;
		uintreg			getStructSize() const;
		TypeCrc			getStructType() const;

		template< class T >
		ArrayView< T >	getArray() const;
		const void*		getArrayData() const;
		uintreg			getArrayElementSize() const;
		uintreg			getArrayLength() const;
		TypeCrc			getArrayType() const;

		void			setVoid();
		void			setBoolean( bool value );
		void			setInteger( sint32 value );
		void			setUnsigned( uint32 value );
		void			setString( const char* pValue );
		void			setPercentValue( PercentValue value );
		void			setDeviceId( DeviceId value );
		void			setInstanceId( InstanceId value );
		void			setTypeCrc( TypeCrc value );
		void			setStruct( const void* pData, uintreg size, TypeCrc type );
		void			setArray( const void* pData, uintreg elementSize, uintreg length, TypeCrc type );

	private:

		Value			m_value;
		DynamicString	m_string;
		Vector< uint8 >	m_buffer;
	};
}

#include "conct_value_high.inl"
