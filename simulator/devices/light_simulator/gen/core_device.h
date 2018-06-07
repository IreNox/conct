#pragma once

#include "conct_core.h"
#include "conct_array_view.h"
#include "core_instance.h"

namespace conct
{
	class Device
	{
	public:

		static const TypeCrc s_typeCrc = 32636;

		virtual const char* getName() const = 0;
		virtual Guid getGuid() const = 0;
		virtual ArrayView< Instance > getInstances() const = 0;

		virtual ArrayView< InstanceId > FindInstances( TypeCrc type ) = 0;
		virtual InstanceId FinstFirstInstance( TypeCrc type ) = 0;
	};
}
