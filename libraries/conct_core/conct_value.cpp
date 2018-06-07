#include "conct_value.h"

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

	PercentValue Value::getPercentValue() const
	{
		CONCT_ASSERT( type == ValueType_PercentValue );
		return data.percent;
	}

	Guid Value::setGuid() const
	{
		CONCT_ASSERT( type == ValueType_Guid );
		return data.guid;
	}

	DeviceId Value::getDeviceId() const
	{
		CONCT_ASSERT( type == ValueType_Device );
		return data.device;
	}

	InstanceId Value::getInstanceId() const
	{
		CONCT_ASSERT( type == ValueType_Instance );
		return data.instance;
	}

	TypeCrc Value::getTypeCrc() const
	{
		CONCT_ASSERT( type == ValueType_Type );
		return data.type;
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
		type = ValueType_Device;
		data.device = value;
	}

	void Value::setInstanceId( InstanceId value )
	{
		type = ValueType_Instance;
		data.instance = value;
	}

	void Value::setTypeCrc( TypeCrc value )
	{
		type = ValueType_Type;
		data.type = value;
	}
}