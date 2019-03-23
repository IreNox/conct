#pragma once

#include "conct_core.h"

namespace conct
{
	struct DeviceConnection
	{
		static const TypeCrc s_typeCrc = 24671;

		DeviceId DeviceStatus;
		DeviceId DeviceId;
	};
}
