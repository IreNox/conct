#pragma once

#include "conct_core.h"

namespace conct
{
	enum class ValueType : uint8
	{
		Void,
		Boolean,
		Integer,
		Unsigned,
		Decimal,
		PercentValue,
		DeviceId,
		InstanceId,
		TypeCrc,
		String,
		Struct,
		Array,

		Count
	};

	const char*	getValueTypeName( ValueType value );
}