#pragma once

#include "conct_runtime.h"

namespace conct
{
	struct RuntimeHighStoredDevice
	{
		uint32					serialNumber;
		CryptoKey				key;
		CryptoCounter			counter;
	};

	struct RuntimeHighDeviceDatabase
	{
		uint32					deviceCount;
		RuntimeHighStoredDevice	devices[ 1u ];
	};

}