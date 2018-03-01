#include "conct_device.h"

#include "conct_instance.h"
#include "conct_proxy.h"

namespace conct
{
	bool Device::findInstanceForInterface( LocalInstance** ppInstance, TypeCrc typeCrc )
	{
		Array< LocalInstance* > instances;
		getInstances( instances );

		for( size_t i = 0u; i < instances.getCount(); ++i )
		{
			LocalInstance* pInstance = instances[ i ];

			if( pInstance->getProxy()->getTypeCrc() == typeCrc )
			{
				*ppInstance = pInstance;
				return true;
			}
		}

		return false;
	}
}