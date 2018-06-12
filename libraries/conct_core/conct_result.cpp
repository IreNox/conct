#include "conct_result.h"

namespace conct
{
	const char* getResultString( ResultId value )
	{
		static const char* s_aResultIdStrings[] =
		{
			"Unknown",
			"Success",
			"OutOfMemory",
			"TimeOut",
			"Unsupported",
			"AlreadyExists",
			"PathNotFound",
			"NoSuchDevice",
			"NoSuchInstance",
			"NoSuchField",
			"NoSource",
			"NoDestination",
		};
		CONCT_STATIC_ASSERT( CONCT_COUNT( s_aResultIdStrings ) == ResultId_Count );

		CONCT_ASSERT( value < ResultId_Count );
		return s_aResultIdStrings[ value ];
	}
}