#pragma once

#include "conct_core.h"

namespace conct
{
	struct RuntimeMessageBaseHeader
	{
		uint16_t	payloadSize;
		uint8_t		sourceHops;
		uint8_t		destinationHops;
	};
}