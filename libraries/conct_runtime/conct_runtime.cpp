#include "conct_runtime.h"

namespace conct
{
	void Runtime::setup( const Array< Proxy* >& proxies )
	{
		m_proxies = proxies;
	}

	void Runtime::processData( const Array< uint8_t >& data )
	{

	}
}