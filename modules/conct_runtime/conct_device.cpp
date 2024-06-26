#include "conct_device.h"

#include "conct_proxy.h"
#include "conct_runtime.h"

namespace conct
{
	uint32 Device::getSerialNumber() const
	{
		return 7u;
	}

	Device::ConstInstanceView Device::getInstances() const
	{
		Device::ConstInstanceView instances;
		getPublicInstances( instances );
		return instances;
	}

	Device::ConstInstanceView Device::FindInstances( TypeCrc typeCrc )
	{
		ArrayView< const Instance > instances;
		getPublicInstances( instances );

		ArrayView< Instance > resultInstances;
		getEmptyInstances( resultInstances );

		uintreg count = 0u;
		for( uintreg i = 0u; i < instances.getLength(); ++i )
		{
			if( instances[ i ].type != typeCrc )
			{
				continue;
			}

			resultInstances[ count++ ] = instances[ i ];
		}

		return Device::ConstInstanceView( resultInstances.getData(), count );
	}

	Instance Device::FindFirstInstance( TypeCrc typeCrc )
	{
		ArrayView< const Instance > instances;
		getPublicInstances( instances );

		for( uintreg i = 0u; i < instances.getLength(); ++i )
		{
			if( instances[ i ].type == typeCrc )
			{
				return instances[ i ];
			}
		}

		return InvalidInstance;
	}

	const LocalInstance* Device::getInstance( InstanceId instanceId )
	{
		ArrayView< const LocalInstance > instances;
		getLocalInstances( instances );

		if( instanceId > instances.getLength() )
		{
			return nullptr;
		}

		return &instances[ instanceId ];
	}
}