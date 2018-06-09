#pragma once

#include "conct_core.h"

#if CONCT_ENABLED( CONCT_IS_DLL )
#	define SIMULATOR_DLL __declspec( dllimport )
#else
#	define SIMULATOR_DLL __declspec( dllexport )
#endif

namespace conct
{
	class ISimulatorPort;

	class ISimulatorContext
	{
	public:

		virtual void	registerPort( ISimulatorPort* pPort ) = 0;
	};

	SIMULATOR_DLL ISimulatorContext& getSimulatorContext();
}