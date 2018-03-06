#include "conct_value.h"

namespace conct
{
	bool Value::getBoolean() const
	{
		CONCT_ASSERT( type == ValueType_Boolean );
		return data.boolean;
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

	void Value::setPercentValue( PercentValue value )
	{
		type = ValueType_PercentValue;
		data.percent = value;
	}
}