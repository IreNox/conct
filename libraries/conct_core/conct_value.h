#pragma once

#include "conct_core.h"

namespace conct
{
	enum ValueType : uint8_t
	{
		ValueType_Boolean,
		ValueType_Integer,
		ValueType_Unsigned,
		ValueType_String,
		ValueType_PercentValue,
		ValueType_Guid,
		ValueType_Instance
	};

	union ValueData
	{
		bool		boolean;
		int			integer;
		unsigned	unsignedInteger;
		const char*	pString;
		uint16_t	percent;
		uint32_t	guid;
		uint16_t	instance;
	};

	struct Value
	{
		ValueType	type;
		ValueData	data;
	};
}