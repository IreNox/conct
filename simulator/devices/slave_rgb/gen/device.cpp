#include "device.h"

#include "conct_array.h"

namespace conct
{
	void DeviceLightSimulator::getProxies( Array< ProxyRegistry >& proxies ) const
	{
		static const ProxyRegistry s_proxies[] =
		{
		};

		proxies.set( s_proxies, CONCT_COUNT( s_proxies ) );
	}
}
