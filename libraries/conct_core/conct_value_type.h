#pragma once

#include "conct_core.h"

namespace conct
{
	enum ValueType : uint8
	{
		ValueType_Void,
		ValueType_Boolean,
		ValueType_Integer,
		ValueType_Unsigned,
		ValueType_String,
		ValueType_PercentValue,
		ValueType_Guid,
		ValueType_DeviceId,
		ValueType_InstanceId,
		ValueType_TypeCrc,
		ValueType_Struct,
		ValueType_Array,

		ValueType_Count
	};
	const char*	getValueTypeName( ValueType value );
}