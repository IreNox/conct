#pragma once

#include "conct_core.h"

namespace conct
{
	enum ResultId : uint8
	{
		ResultId_Unknown,

		ResultId_Success,

		ResultId_OutOfMemory,
		ResultId_Timeout,
		ResultId_Unsupported,
		ResultId_NoSuchDevice,
		ResultId_NoSuchInstance,
		ResultId_NoSuchField,
		ResultId_NoSource,
		ResultId_NoDestination,

		ResultId_Count
	};

	struct ResultBase
	{
		ResultId		result;

		inline bool		isSuccess() const { return result == ResultId_Success; }
		inline bool		isError() const { return result != ResultId_Success; }

		inline bool		isBusy() const { return result == ResultId_Timeout; }
		inline bool		isDone() const { return result != ResultId_Timeout; }
	};

	template< class TValue >
	struct Result : public ResultBase
	{
		TValue			value;
	};

	template< >
	struct Result< void > : public ResultBase
	{
	};
}