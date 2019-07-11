#pragma once

#include "conct_runtime.h"

namespace conct
{
	struct RuntimeHighStoredDevice
	{
		uint32					serialNumber;
		uint16					hash;
#if CONCT_ENABLED( CONCT_RUNTIME_USE_CRYPTO )
		CryptoKey				key;
		CryptoCounter			counter;
#endif
	};

	struct RuntimeHighDeviceDatabase
	{
		uint32					deviceCount;
		RuntimeHighStoredDevice	devices[ 1u ];
	};

}