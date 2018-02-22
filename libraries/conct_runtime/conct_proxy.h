#pragma once

#include "conct_array.h"
#include "conct_value.h"

namespace conct
{
	class Proxy
	{
	public:

		virtual bool	getProperty( Value* pTargetValue, const char* pName ) = 0;
		virtual bool	setProperty( const char* pName, const Value& value ) = 0;

		virtual bool	callFunction( Value* pReturnValue, const char* pName, const Array< Value >& parameters ) = 0;
	};
}