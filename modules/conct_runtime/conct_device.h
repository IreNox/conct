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

		virtual					~Device() { }

		virtual const char*		getName() const = 0;
		uint32					getSerialNumber() const;
		ArrayView< Instance >	getInstances() const;

		const LocalInstance*	getInstance( InstanceId instanceId );

		// interface functions
		ArrayView< Instance >	FindInstances( TypeCrc typeCrc );
		Instance				FindFirstInstance( TypeCrc typeCrc );

	protected:

		virtual void			getEmptyInstances( Array< Instance >& instances ) = 0;
		virtual void			getPublicInstances( ArrayView< Instance >& instances ) const = 0;
		virtual void			getLocalInstances( ArrayView< LocalInstance >& instances ) = 0;
	};
}