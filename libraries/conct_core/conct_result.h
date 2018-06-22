#pragma once

#include "conct_core.h"

namespace conct
{
	enum ResultId : uint8
	{
		ResultId_Unknown,

		ResultId_Success,

		ResultId_OutOfMemory,
		ResultId_TimeOut,
		ResultId_Unsupported,
		ResultId_ConnectionLost,
		ResultId_PermissionDenied,
		ResultId_AlreadyExists,
		ResultId_PathNotFound,
		ResultId_NoSuchDevice,
		ResultId_NoSuchInstance,
		ResultId_NoSuchField,
		ResultId_NoSource,
		ResultId_NoDestination,

		ResultId_Count
	};
	const char* getResultString( ResultId value );

	struct ResultBase
	{
		ResultId			result;

		inline bool			isSuccess() const { return result == ResultId_Success; }
		inline bool			isFailure() const { return result != ResultId_Success; }

		inline bool			isBusy() const { return result == ResultId_TimeOut; }
		inline bool			isDone() const { return result != ResultId_TimeOut; }

		inline const char*	getResultString() const { return ::conct::getResultString( result ); }
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

	template< class T >
	inline Result< T > createSuccessResult( const T& value )
	{
		Result< T > result;
		result.result = ResultId_Success;
		result.value = value;
		return result;
	}

	inline Result< void > createSuccessResult()
	{
		Result< void > result;
		result.result = ResultId_Success;
		return result;
	}

	template< class T >
	inline Result< T > createFailureResult( ResultId resultId )
	{
		Result< T > result;
		result.result = resultId;
		return result;
	}

	template< >
	inline Result< void > createFailureResult< void >( ResultId resultId )
	{
		Result< void > result;
		result.result = resultId;
		return result;
	}
}