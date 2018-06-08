#include "conct_device.h"

#include "conct_proxy.h"
#include "conct_runtime.h"

namespace conct
{
	struct Instance
	{
		InstanceId Id;
		TypeCrc Type;
	};

	Guid Device::getGuid() const
	{
		return Guid( this );
	}

	ArrayView< Instance > Device::getInstances() const
	{
		ArrayView< Instance > instances;
		getPublicInstances( instances );
		return instances;
	}

	ArrayView< Instance > Device::findInstances( TypeCrc typeCrc )
	{
		ArrayView< Instance > instances;
		getPublicInstances( instances );

		Array< Instance > resultInstances;
		getEmptyInstances( resultInstances );

		uintreg count = 0u;
		for( size_t i = 0u; i < instances.getCount(); ++i )
		{
			if( instances[ i ].Type != typeCrc )
			{
				continue;
			}

			resultInstances[ count++ ] = instances[ i ];
		}

		return ArrayView< Instance >( resultInstances.getData(), count );
	}

	InstanceId Device::findFirstInstance( TypeCrc typeCrc )
	{
		ArrayView< Instance > instances;
		getPublicInstances( instances );

		for( size_t i = 0u; i < instances.getCount(); ++i )
		{
			if( instances[ i ].Type == typeCrc )
			{
				return instances[ i ].Id;
			}
		}

		return InvalidInstanceId;
	}

	const LocalInstance* Device::getInstance( InstanceId instanceId )
	{
		ArrayView< LocalInstance > instances;
		getLocalInstances( instances );

		if( instanceId > instances.getCount() )
		{
			return nullptr;
		}

		return &instances[ instanceId ];
	}
}