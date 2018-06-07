#pragma once

#include "conct_array_view.h"

namespace conct
{
	struct LocalInstance;

	class Device2
	{
	public:

		virtual void getInstances( ArrayView< LocalInstance >& instances ) = 0;
		virtual bool findInstanceForInterface( LocalInstance* pInstance, TypeCrc typeCrc );
	};
}