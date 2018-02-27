#pragma once

#include "conct_array.h"
#include "conct_value.h"

namespace conct
{
	class Proxy
	{
	public:

		TypeCrc			getTypeCrc() const { return m_typeCrc; }

		virtual bool	getProperty( Value* pTargetValue, const char* pName ) = 0;
		virtual bool	setProperty( const char* pName, const Value& value ) = 0;

		virtual bool	callFunction( Value* pReturnValue, const char* pName, const Array< Value >& parameters ) = 0;

	private:

		TypeCrc			m_typeCrc;
	};
}