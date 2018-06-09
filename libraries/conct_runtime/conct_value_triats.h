#pragma once

#include "conct_core.h"

namespace conct
{
	template< class T >
	struct ValueTypeTraits
	{
		static CONCT_FORCE_INLINE TypeCrc getTypeCrc();
	};
}

#include "conct_value_triats.inl"