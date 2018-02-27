#pragma once

#include "conct_array.h"

namespace conct
{
	struct ProxyRegistry;
	class LocalInstance;

	class Device
	{
	public:

		virtual void getInstances( Array< LocalInstance* >& instances ) = 0;
		virtual bool findInstanceForInterface( LocalInstance** ppInstance, TypeCrc typeCrc );
	};
}