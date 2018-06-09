#include "conct_value.h"

#include "conct_functions.h"

namespace conct
{
	bool Value::getBoolean() const
	{
		CONCT_ASSERT( type == ValueType_Boolean );
		return data.boolean;
	}

	sint32 Value::getInteger() const
	{
		CONCT_ASSERT( type == ValueType_Integer );
		return data.integer;
	}

	uint32 Value::getUnsigned() const
	{
		CONCT_ASSERT( type == ValueType_Unsigned );
		return data.unsignedInteger;
	}

	const char* Value::getString() const
	{
		CONCT_ASSERT( type == ValueType_String );
		if( data.string.offset == 0u )
		{
			return nullptr;
		}

		return addPointerCast< char >( this, data.string.offset );
	}

	PercentValue Value::getPercentValue() const
	{
		CONCT_ASSERT( type == ValueType_PercentValue );
		return data.percent;
	}

	Guid Value::getGuid() const
	{
		CONCT_ASSERT( type == ValueType_Guid );
		return data.guid;
	}

	DeviceId Value::getDeviceId() const
	{
		CONCT_ASSERT( type == ValueType_DeviceId );
		return data.device;
	}

	Instance Value::getInstance() const
	{
		CONCT_ASSERT( type == ValueType_Instance );
		return data.instance;
	}

	TypeCrc Value::getTypeCrc() const
	{
		CONCT_ASSERT( type == ValueType_TypeCrc );
		return data.type;
	}

	const void* Value::getStructData() const
	{
		CONCT_ASSERT( type == ValueType_Struct );
		if( data.structure.offset == 0u )
		{
			return nullptr;
		}

		return addPointer( this, data.structure.offset );
	}

	uintreg Value::getStructSize() const
	{
		CONCT_ASSERT( type == ValueType_Struct );
		return data.structure.size;
	}

	TypeCrc Value::getStructType() const
	{
		CONCT_ASSERT( type == ValueType_Struct );
		return data.structure.type;
	}

	const void* Value::getArrayData() const
	{
		CONCT_ASSERT( type == ValueType_Array );
		if( data.array.offset == 0u )
		{
			return nullptr;
		}

		return addPointer( this, data.array.offset );
	}

	uintreg Value::getArrayElementSize() const
	{
		CONCT_ASSERT( type == ValueType_Array );
		return data.array.size;
	}

	uintreg Value::getArrayCount() const
	{
		CONCT_ASSERT( type == ValueType_Array );
		return data.array.count;
	}

	TypeCrc Value::getArrayType() const
	{
		CONCT_ASSERT( type == ValueType_Array );
		return data.array.type;
	}

	void Value::setVoid()
	{
		type = ValueType_Void;
	}

	void Value::setBoolean( bool value )
	{
		type = ValueType_Boolean;
		data.boolean = value;
	}

	void Value::setInteger( sint32 value )
	{
		type = ValueType_Integer;
		data.integer = value;
	}

	void Value::setUnsigned( uint32 value )
	{
		type = ValueType_Unsigned;
		data.unsignedInteger = value;
	}

	void Value::setPercentValue( PercentValue value )
	{
		type = ValueType_PercentValue;
		data.percent = value;
	}

	void Value::setGuid( Guid value )
	{
		type = ValueType_Guid;
		data.guid = value;
	}

	void Value::setDeviceId( DeviceId value )
	{
		type = ValueType_DeviceId;
		data.device = value;
	}

	void Value::setInstance( Instance value )
	{
		type = ValueType_Instance;
		data.instance = value;
	}

	void Value::setTypeCrc( TypeCrc value )
	{
		type = ValueType_TypeCrc;
		data.type = value;
	}
}