#include "conct_device.h"

#include "conct_proxy.h"
#include "conct_runtime.h"

namespace conct
{
	Guid Device::getGuid() const
	{
		return Guid( 42u );
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
		for( uintreg i = 0u; i < instances.getLength(); ++i )
		{
			if( instances[ i ].type != typeCrc )
			{
				continue;
			}

			resultInstances[ count++ ] = instances[ i ];
		}

		return ArrayView< Instance >( resultInstances.getData(), count );
	}

	Instance Device::findFirstInstance( TypeCrc typeCrc )
	{
		ArrayView< Instance > instances;
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
		ArrayView< LocalInstance > instances;
		getLocalInstances( instances );

		if( instanceId > instances.getLength() )
		{
			return nullptr;
		}

		return &instances[ instanceId ];
	}
}