#pragma once

#include "conct_array.h"
#include "conct_array_view.h"

namespace conct
{
	struct LocalInstance;
	struct Instance;

	class Device
	{
	public:

		virtual const char*		getName() const = 0;
		Guid					getGuid() const;
		ArrayView< Instance >	getInstances() const;

		ArrayView< Instance >	findInstances( TypeCrc typeCrc );
		Instance				findFirstInstance( TypeCrc typeCrc );

		const LocalInstance*	getInstance( InstanceId instanceId );

	protected:

		virtual void			getEmptyInstances( Array< Instance >& instances ) = 0;
		virtual void			getPublicInstances( ArrayView< Instance >& instances ) const = 0;
		virtual void			getLocalInstances( ArrayView< LocalInstance >& instances ) = 0;
	};
}