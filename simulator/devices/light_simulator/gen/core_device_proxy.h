#pragma once

#include "conct_proxy.h"

namespace conct
{
	class DeviceProxy : public Proxy
	{
	public:

		DeviceProxy();

		virtual bool getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, const char* pName ) const CONCT_OVERRIDE_FINAL;
		virtual bool setProperty( void* pInstance, const char* pName, const Value& value ) CONCT_OVERRIDE_FINAL;

		virtual bool callFunction( ValueBuilder& targetValueBuilder, void* pInstance, const char* pName, const ArrayView< Value >& parameters ) CONCT_OVERRIDE_FINAL;
	};
}
