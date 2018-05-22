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
		ValueType_Instance,
		ValueType_Array
	};

	union ValueData
	{
		bool			boolean;
		int				integer;
		unsigned		unsignedInteger;
		const char*		pString;
		PercentValue	percent;
		uint32			guid;
		InstanceId		instance;
	};

	struct Value
	{
		ValueType		type;
		ValueData		data;

		bool			getBoolean() const;
		PercentValue	getPercentValue() const;

		void			setBoolean( bool value );
		void			setPercentValue( PercentValue value );
	};
}