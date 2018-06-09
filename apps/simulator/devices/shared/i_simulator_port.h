#pragma once

#include "conct_core.h"

namespace conct
{
	class ISimulatorPort
	{
	public:

		virtual void	setCounterpartPort( ISimulatorPort* pPort ) = 0;

		virtual void	pushData( const void* pData, uintreg dataSize ) = 0;
	};
}