#pragma once

#include "conct_array.h"

namespace conct
{
	struct ProxyRegistry;
	class Instance;

	class Device
	{
	public:

		virtual void getInstances( Array< Instance* >& instances ) = 0;
		virtual bool findInstanceForInterface( Instance** ppInstance, TypeCrc typeCrc );
	};
}