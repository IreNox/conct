#pragma once

#include "conct_core.h"

#include <tiki/tiki_array_view.h>

namespace conct
{
	struct LocalInstance;
	struct Instance;

	class Device
	{
	public:

		using ConstInstanceView = ArrayView< const Instance >;

		virtual					~Device() { }

		virtual const char*		getName() const = 0;
		uint32					getSerialNumber() const;
		ConstInstanceView		getInstances() const;

		const LocalInstance*	getInstance( InstanceId instanceId );

		// interface functions
		ConstInstanceView		FindInstances( TypeCrc typeCrc );
		Instance				FindFirstInstance( TypeCrc typeCrc );

	protected:

		virtual void			getEmptyInstances( ArrayView< Instance >& instances ) = 0;
		virtual void			getPublicInstances( ConstInstanceView& instances ) const = 0;
		virtual void			getLocalInstances( ArrayView< const LocalInstance >& instances ) = 0;
	};
}