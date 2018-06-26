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
		Guid			getGuid() const;
		Instance		getInstance() const;
		DeviceId		getDeviceId() const;
		TypeCrc			getTypeCrc() const;

		const char*		getString() const;

		const void*		getStructData() const;
		uint16			getStructSize() const;
		TypeCrc			getStructType() const;

		const void*		getArrayData() const;
		uint8			getArrayElementSize() const;
		uint8			getArrayCount() const;
		TypeCrc			getArrayType() const;

		void			setVoid();
		void			setBoolean( bool value );
		void			setInteger( sint32 value );
		void			setUnsigned( uint32 value );
		void			setPercentValue( PercentValue value );
		void			setGuid( Guid value );
		void			setDeviceId( DeviceId value );
		void			setInstance( Instance value );
		void			setTypeCrc( TypeCrc value );

		void			setString( uint16 offset );
		void			setStruct( uint16 offset, uint16 size, TypeCrc typeCrc );
		void			setArray( uint16 offset, uint8 count, uint8 size, TypeCrc typeCrc );
	};
}