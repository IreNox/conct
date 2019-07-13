#pragma once

#include "conct_proxy.h"

namespace conct
{
	class LightProxy : public Proxy
	{
	public:

		LightProxy();

		virtual bool getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, uint16 nameCrc ) const CONCT_OVERRIDE_FINAL;
		virtual bool setProperty( void* pInstance, uint16 nameCrc, const Value& value ) const CONCT_OVERRIDE_FINAL;

		virtual bool callFunction( ValueBuilder& targetValueBuilder, void* pInstance, uint16 nameCrc, const ArrayView< Value >& parameters ) const CONCT_OVERRIDE_FINAL;
	};
}
