#pragma once

#include "conct_core.h"

namespace conct
{
	struct Instance
	{
		static const TypeCrc s_typeCrc = 36718;

		InstanceId Id;
		TypeCrc Type;
	};
}
