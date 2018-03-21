#pragma once

#if is_dll
#	define SIMULATOR_DLL __declspec( dllimport )
#else
#	define SIMULATOR_DLL __declspec( dllexport )
#endif

namespace conct
{
	class SimulatorContext
	{
	public:

	private:

	};

	SIMULATOR_DLL SimulatorContext& getSimulatorContext();
}