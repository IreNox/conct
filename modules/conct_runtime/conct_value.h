#pragma once

#include "conct_core.h"
#include "conct_value_type.h"

namespace conct
{
	struct Value
	{
		ValueType		type;
		uint8			data[ 6u ];

		bool			getBoolean() const;
		sint32			getInteger() const;
		uint32			getUnsigned() const;
		PercentValue	getPercentValue() const;
		InstanceId		getInstanceId() const;
		DeviceId		getDeviceId() const;
		TypeCrc			getTypeCrc() const;

		const char*		getString() const;

		const void*		getStructData() const;
		uint16			getStructSize() const;
		TypeCrc			getStructType() const;

		const void*		getArrayData() const;
		uint8			getArrayElementSize() const;
		uint8			getArrayLength() const;
		TypeCrc			getArrayType() const;

		void			setVoid();
		void			setBoolean( bool value );
		void			setInteger( sint32 value );
		void			setUnsigned( uint32 value );
		void			setPercentValue( PercentValue value );
		void			setDeviceId( DeviceId value );
		void			setInstanceId( InstanceId value );
		void			setTypeCrc( TypeCrc value );

		void			setString( uint16 offset );
		void			setStruct( uint16 offset, uint16 size, TypeCrc typeCrc );
		void			setArray( uint16 offset, uint8 size, uint8 length, TypeCrc typeCrc );
	};
}