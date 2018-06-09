#include "conct_value_type.h"

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
}