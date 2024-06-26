#include "conct_value_type.h"

#include <tiki/tiki_types.h>

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
			"Decimal",
			"PercentValue",
			"DeviceId",
			"InstanceId",
			"TypeCrc",
			"String",
			"Structure",
			"Array"
		};
		TIKI_STATIC_ASSERT( TIKI_ARRAY_COUNT( s_aValueTypeNames ) == (uintsize)ValueType::Count );

		TIKI_ASSERT( value < ValueType::Count );
		return s_aValueTypeNames[ (uintsize)value ];
	}
}