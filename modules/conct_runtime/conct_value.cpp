#include "conct_value.h"

#include <tiki/tiki_memory.h>

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
		uint8				size;
		uint8				length;
		TypeCrc				type;
	};

	union ValueData
	{
		bool				boolean;
		sint32				integer;
		uint32				unsignedInteger;
		PercentValue		percent;
		DeviceId			device;
		InstanceId			instanceId;
		TypeCrc				type;
		StringValueData		string;
		StructValueData		structure;
		ArrayValueData		array;
	};

	ValueData getData( const uint8 data[ 6u ] )
	{
		ValueData result;
		memory::copy( &result, data, 6u );
		return result;
	}

	void setData( uint8 data[ 6u ], const ValueData& sourceData )
	{
		memory::copy( data, &sourceData, 6u );
	}

	bool Value::getBoolean() const
	{
		TIKI_ASSERT( type == ValueType::Boolean );
		return getData( data ).boolean;
	}

	sint32 Value::getInteger() const
	{
		TIKI_ASSERT( type == ValueType::Integer );
		return getData( data ).integer;
	}

	uint32 Value::getUnsigned() const
	{
		TIKI_ASSERT( type == ValueType::Unsigned );
		return getData( data ).unsignedInteger;
	}

	const char* Value::getString() const
	{
		TIKI_ASSERT( type == ValueType::String );
		const ValueData valueData = getData( data );
		if( valueData.string.offset == 0u )
		{
			return nullptr;
		}

		return addPointerCast< char >( this, valueData.string.offset );
	}

	PercentValue Value::getPercentValue() const
	{
		TIKI_ASSERT( type == ValueType::PercentValue );
		return getData( data ).percent;
	}

	DeviceId Value::getDeviceId() const
	{
		TIKI_ASSERT( type == ValueType::DeviceId );
		return getData( data ).device;
	}

	InstanceId Value::getInstanceId() const
	{
		TIKI_ASSERT( type == ValueType::InstanceId );
		return getData( data ).instanceId;
	}

	TypeCrc Value::getTypeCrc() const
	{
		TIKI_ASSERT( type == ValueType::TypeCrc );
		return getData( data ).type;
	}

	const void* Value::getStructData() const
	{
		TIKI_ASSERT( type == ValueType::Struct );
		const ValueData valueData = getData( data );
		if( valueData.structure.offset == 0u )
		{
			return nullptr;
		}

		return addPointer( this, valueData.structure.offset );
	}

	uint16 Value::getStructSize() const
	{
		TIKI_ASSERT( type == ValueType::Struct );
		return getData( data ).structure.size;
	}

	TypeCrc Value::getStructType() const
	{
		TIKI_ASSERT( type == ValueType::Struct );
		return getData( data ).structure.type;
	}

	const void* Value::getArrayData() const
	{
		TIKI_ASSERT( type == ValueType::Array );
		const ValueData valueData = getData( data );
		if( valueData.array.offset == 0u )
		{
			return nullptr;
		}

		return addPointer( this, valueData.array.offset );
	}

	uint8 Value::getArrayElementSize() const
	{
		TIKI_ASSERT( type == ValueType::Array );
		return getData( data ).array.size;
	}

	uint8 Value::getArrayLength() const
	{
		TIKI_ASSERT( type == ValueType::Array );
		return getData( data ).array.length;
	}

	TypeCrc Value::getArrayType() const
	{
		TIKI_ASSERT( type == ValueType::Array );
		return getData( data ).array.type;
	}

	void Value::setVoid()
	{
		type = ValueType::Void;
	}

	void Value::setBoolean( bool value )
	{
		type = ValueType::Boolean;
		ValueData valueData;
		valueData.boolean = value;
		setData( data, valueData );
	}

	void Value::setInteger( sint32 value )
	{
		type = ValueType::Integer;
		ValueData valueData;
		valueData.integer = value;
		setData( data, valueData );
	}

	void Value::setUnsigned( uint32 value )
	{
		type = ValueType::Unsigned;
		ValueData valueData;
		valueData.unsignedInteger = value;
		setData( data, valueData );
	}

	void Value::setPercentValue( PercentValue value )
	{
		type = ValueType::PercentValue;
		ValueData valueData;
		valueData.percent = value;
		setData( data, valueData );
	}

	void Value::setDeviceId( DeviceId value )
	{
		type = ValueType::DeviceId;
		ValueData valueData;
		valueData.device = value;
		setData( data, valueData );
	}

	void Value::setInstanceId( InstanceId value )
	{
		type = ValueType::InstanceId;
		ValueData valueData;
		valueData.instanceId = value;
		setData( data, valueData );
	}

	void Value::setTypeCrc( TypeCrc value )
	{
		type = ValueType::TypeCrc;
		ValueData valueData;
		valueData.type = value;
		setData( data, valueData );
	}

	void Value::setString( uint16 offset )
	{
		type = ValueType::String;
		ValueData valueData;
		valueData.string.offset = offset;
		setData( data, valueData );
	}

	void Value::setStruct( uint16 offset, uint16 size, TypeCrc typeCrc )
	{
		type = ValueType::Struct;
		ValueData valueData;
		valueData.structure.offset = offset;
		valueData.structure.size = size;
		valueData.structure.type = typeCrc;
		setData( data, valueData );
	}

	void Value::setArray( uint16 offset, uint8 size, uint8 length, TypeCrc typeCrc )
	{
		type = ValueType::Array;
		ValueData valueData;
		valueData.array.offset = offset;
		valueData.array.size = size;
		valueData.array.length = length;
		valueData.array.type = typeCrc;
		setData( data, valueData );
	}
}