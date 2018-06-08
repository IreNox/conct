#pragma once

#include "conct_array_view.h"

namespace conct
{
	class Router
	{
	public:

		ArrayView< DeviceId > getConnectedDevices() const;
	};
}