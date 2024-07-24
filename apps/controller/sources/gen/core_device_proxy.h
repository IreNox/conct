#pragma once

#include "conct_proxy.h"

namespace conct
{
	class DeviceProxy : public Proxy
	{
	public:

		DeviceProxy();

		virtual bool getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, uint16 nameCrc ) const TIKI_OVERRIDE_FINAL;
		virtual bool setProperty( void* pInstance, uint16 nameCrc, const Value& value ) const TIKI_OVERRIDE_FINAL;

		virtual bool callFunction( ValueBuilder& targetValueBuilder, void* pInstance, uint16 nameCrc, const ArrayView< const Value >& parameters ) const TIKI_OVERRIDE_FINAL;
	};
}
