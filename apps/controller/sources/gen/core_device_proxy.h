#pragma once

#include "conct_proxy.h"

namespace conct
{
	class DeviceProxy : public Proxy
	{
	public:

		DeviceProxy();

		virtual bool getProperty( Value& targetValue, const void* pInstance, uint16 nameCrc ) const override final;
		virtual bool setProperty( void* pInstance, uint16 nameCrc, const Value& value ) const override final;

		virtual bool callFunction( Value& targetValue, void* pInstance, uint16 nameCrc, const ConstArrayView< Value >& parameters ) const override final;
	};
}
