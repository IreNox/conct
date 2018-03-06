#pragma once

#include "conct_proxy.h"

namespace conct
{
	class DimmerProxy : public Proxy
	{
	public:

		DimmerProxy();

		virtual bool getProperty( Value& targetValue, const void* pInstance, const char* pName ) const CONCT_OVERRIDE_FINAL;
		virtual bool setProperty( void* pInstance, const char* pName, const Value& value ) CONCT_OVERRIDE_FINAL;

		virtual bool callFunction( Value& returnValue, void* pInstance, const char* pName, const Array< Value >& parameters ) CONCT_OVERRIDE_FINAL;
	};
}
