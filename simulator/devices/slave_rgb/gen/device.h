#pragma once

#include "conct_array.h"
#include "conct_core.h"


namespace conct
{
	struct ProxyRegistry;

	class DeviceLightSimulator
	{
	public:

		void getProxies( Array< ProxyRegistry >& proxies ) const;

	private:

	};
}
