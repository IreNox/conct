#pragma once

#include "conct_core.h"
#include "conct_array.h"

namespace conct
{
	class Proxy;

	class Runtime
	{
	public:

		void	setup( const Array< Proxy* >& proxies );

		void	processData( const Array< uint8_t >& data );

	private:

		Array< Proxy* >	m_proxies;
	};
}
