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
			"ConnectionLost",
			"PermissionDenied",
			"AlreadyExists",
			"PathNotFound",
			"NoSuchDevice",
			"NoSuchInstance",
			"NoSuchField",
			"NoSource",
			"NoDestination",
			"KeyExchangeFailed",
		};
		CONCT_STATIC_ASSERT( CONCT_COUNT( s_aResultIdStrings ) == ResultId_Count );

		CONCT_ASSERT( value < ResultId_Count );
		return s_aResultIdStrings[ value ];
	}
}