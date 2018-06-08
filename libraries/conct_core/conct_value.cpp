#include "conct_value.h"

#include "conct_functions.h"

namespace conct
{
	const char* getValueTypeName( ValueType value )
	{
		static const char* s_aValueTypeNames[] =
		{
			"Void",
			"Boolean",
			"Integer",
			"Unsigned",
			"String",
			"PercentValue",
			"Guid",
			"Device",
			"Instance",
			"Type",
			"Structure",
			"Array"
		};
		CONCT_STATIC_ASSERT( CONCT_COUNT( s_aValueTypeNames ) == ValueType_Count );

		CONCT_ASSERT( value < ValueType_Count );
		return s_aValueTypeNames[ value ];
	}

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

	InstanceId Value::getInstanceId() const
	{
		CONCT_ASSERT( type == ValueType_InstanceId );
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

	void Value::setInstanceId( InstanceId value )
	{
		type = ValueType_InstanceId;
		data.instance = value;
	}

	void Value::setTypeCrc( TypeCrc value )
	{
		type = ValueType_TypeCrc;
		data.type = value;
	}
}