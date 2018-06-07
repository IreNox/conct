#include "conct_device.h"

#include "conct_proxy.h"
#include "conct_runtime.h"

namespace conct
{
	bool Device2::findInstanceForInterface( LocalInstance* pInstance, TypeCrc typeCrc )
	{
		ArrayView< LocalInstance > instances;
		getInstances( instances );

		for( uintreg i = 0u; i < instances.getCount(); ++i )
		{
			const LocalInstance& instance = instances[ i ];

			if( instance.pProxy->getTypeCrc() == typeCrc )
			{
				*pInstance = instance;
				return true;
			}
		}

		return false;
	}
}