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
}