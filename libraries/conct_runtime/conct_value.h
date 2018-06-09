#pragma once

#include "conct_core.h"
#include "conct_value_type.h"

namespace conct
{
	struct StringValueData
	{
		uint16				offset;
	};

	struct StructValueData
	{
		uint16				offset;
		uint16				size;
		TypeCrc				type;
	};

	struct ArrayValueData
	{
		uint16				offset;
		uint8				count;
		uint8				size;
		TypeCrc				type;
	};

	union ValueData
	{
		bool				boolean;
		sint32				integer;
		uint32				unsignedInteger;
		PercentValue		percent;
		Guid				guid;
		DeviceId			device;
		Instance			instance;
		TypeCrc				type;
		StringValueData		string;
		StructValueData		structure;
		ArrayValueData		array;
	};

	struct Value
	{
		ValueType		type;
		ValueData		data;

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
		uintreg			getStructSize() const;
		TypeCrc			getStructType() const;

		const void*		getArrayData() const;
		uintreg			getArrayElementSize() const;
		uintreg			getArrayCount() const;
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
	};
}