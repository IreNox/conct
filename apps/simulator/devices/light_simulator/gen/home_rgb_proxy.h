#pragma once

#include "conct_proxy.h"

namespace conct
{
	class RGBProxy : public Proxy
	{
	public:

		RGBProxy();

		virtual bool getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, const char* pName ) const CONCT_OVERRIDE_FINAL;
		virtual bool setProperty( void* pInstance, const char* pName, const Value& value ) const CONCT_OVERRIDE_FINAL;

		virtual bool callFunction( ValueBuilder& targetValueBuilder, void* pInstance, const char* pName, const ArrayView< Value >& parameters ) const CONCT_OVERRIDE_FINAL;
	};
}
