#include "conct_value.h"

#include "conct_functions.h"
#include "conct_memory.h"

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
		Guid				guid;
		DeviceId			device;
		Instance			instance;
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
		CONCT_ASSERT( type == ValueType_Boolean );
		return getData( data ).boolean;
	}

	sint32 Value::getInteger() const
	{
		CONCT_ASSERT( type == ValueType_Integer );
		return getData( data ).integer;
	}

	uint32 Value::getUnsigned() const
	{
		CONCT_ASSERT( type == ValueType_Unsigned );
		return getData( data ).unsignedInteger;
	}

	const char* Value::getString() const
	{
		CONCT_ASSERT( type == ValueType_String );
		const ValueData valueData = getData( data );
		if( valueData.string.offset == 0u )
		{
			return nullptr;
		}

		return addPointerCast< char >( this, valueData.string.offset );
	}

	PercentValue Value::getPercentValue() const
	{
		CONCT_ASSERT( type == ValueType_PercentValue );
		return getData( data ).percent;
	}

	Guid Value::getGuid() const
	{
		CONCT_ASSERT( type == ValueType_Guid );
		return getData( data ).guid;
	}

	DeviceId Value::getDeviceId() const
	{
		CONCT_ASSERT( type == ValueType_DeviceId );
		return getData( data ).device;
	}

	Instance Value::getInstance() const
	{
		CONCT_ASSERT( type == ValueType_Instance );
		return getData( data ).instance;
	}

	TypeCrc Value::getTypeCrc() const
	{
		CONCT_ASSERT( type == ValueType_TypeCrc );
		return getData( data ).type;
	}

	const void* Value::getStructData() const
	{
		CONCT_ASSERT( type == ValueType_Struct );
		const ValueData valueData = getData( data );
		if( valueData.structure.offset == 0u )
		{
			return nullptr;
		}

		return addPointer( this, valueData.structure.offset );
	}

	uint16 Value::getStructSize() const
	{
		CONCT_ASSERT( type == ValueType_Struct );
		return getData( data ).structure.size;
	}

	TypeCrc Value::getStructType() const
	{
		CONCT_ASSERT( type == ValueType_Struct );
		return getData( data ).structure.type;
	}

	const void* Value::getArrayData() const
	{
		CONCT_ASSERT( type == ValueType_Array );
		const ValueData valueData = getData( data );
		if( valueData.array.offset == 0u )
		{
			return nullptr;
		}

		return addPointer( this, valueData.array.offset );
	}

	uint8 Value::getArrayElementSize() const
	{
		CONCT_ASSERT( type == ValueType_Array );
		return getData( data ).array.size;
	}

	uint8 Value::getArrayLength() const
	{
		CONCT_ASSERT( type == ValueType_Array );
		return getData( data ).array.length;
	}

	TypeCrc Value::getArrayType() const
	{
		CONCT_ASSERT( type == ValueType_Array );
		return getData( data ).array.type;
	}

	void Value::setVoid()
	{
		type = ValueType_Void;
	}

	void Value::setBoolean( bool value )
	{
		type = ValueType_Boolean;
		ValueData valueData;
		valueData.boolean = value;
		setData( data, valueData );
	}

	void Value::setInteger( sint32 value )
	{
		type = ValueType_Integer;
		ValueData valueData;
		valueData.integer = value;
		setData( data, valueData );
	}

	void Value::setUnsigned( uint32 value )
	{
		type = ValueType_Unsigned;
		ValueData valueData;
		valueData.unsignedInteger = value;
		setData( data, valueData );
	}

	void Value::setPercentValue( PercentValue value )
	{
		type = ValueType_PercentValue;
		ValueData valueData;
		valueData.percent = value;
		setData( data, valueData );
	}

	void Value::setGuid( Guid value )
	{
		type = ValueType_Guid;
		ValueData valueData;
		valueData.guid = value;
		setData( data, valueData );
	}

	void Value::setDeviceId( DeviceId value )
	{
		type = ValueType_DeviceId;
		ValueData valueData;
		valueData.device = value;
		setData( data, valueData );
	}

	void Value::setInstance( Instance value )
	{
		type = ValueType_Instance;
		ValueData valueData;
		valueData.instance = value;
		setData( data, valueData );
	}

	void Value::setTypeCrc( TypeCrc value )
	{
		type = ValueType_TypeCrc;
		ValueData valueData;
		valueData.type = value;
		setData( data, valueData );
	}

	void Value::setString( uint16 offset )
	{
		type = ValueType_String;
		ValueData valueData;
		valueData.string.offset = offset;
		setData( data, valueData );
	}

	void Value::setStruct( uint16 offset, uint16 size, TypeCrc typeCrc )
	{
		type = ValueType_Struct;
		ValueData valueData;
		valueData.structure.offset = offset;
		valueData.structure.size = size;
		valueData.structure.type = typeCrc;
		setData( data, valueData );
	}

	void Value::setArray( uint16 offset, uint8 size, uint8 length, TypeCrc typeCrc )
	{
		type = ValueType_Array;
		ValueData valueData;
		valueData.array.offset = offset;
		valueData.array.size = size;
		valueData.array.length = length;
		valueData.array.type = typeCrc;
		setData( data, valueData );
	}
}