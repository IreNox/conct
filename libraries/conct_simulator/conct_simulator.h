#pragma once

#if CONCT_ENABLED( CONCT_IS_DLL )
#	define CONCT_SIMULATOR_DLL __declspec( dllimport )
#else
#	define CONCT_SIMULATOR_DLL __declspec( dllexport )
#endif

namespace conct
{
	class SimulatorContext;

	CONCT_SIMULATOR_DLL SimulatorContext& getSimulatorContext();
}