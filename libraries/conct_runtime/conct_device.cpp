#include "conct_device.h"

#include "conct_instance.h"
#include "conct_proxy.h"

namespace conct
{
	bool Device::findInstanceForInterface( LocalInstance* pInstance, TypeCrc typeCrc )
	{
		ArrayView< LocalInstance > instances;
		getInstances( instances );

		for( uint8_t i = 0u; i < instances.getCount(); ++i )
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