#pragma once

#include "conct_proxy.h"

namespace conct
{
	class RouterProxy : public Proxy
	{
	public:

		RouterProxy();

		virtual bool getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, uint16 nameCrc ) const override final;
		virtual bool setProperty( void* pInstance, uint16 nameCrc, const Value& value ) const override final;

		virtual bool callFunction( ValueBuilder& targetValueBuilder, void* pInstance, uint16 nameCrc, const ArrayView< const Value >& parameters ) const override final;
	};
}
