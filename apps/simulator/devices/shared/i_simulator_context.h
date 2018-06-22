#pragma once

#include "conct_core.h"

#if CONCT_ENABLED( CONCT_IS_DLL )
#	define SIMULATOR_DLL __declspec( dllimport )
#else
#	define SIMULATOR_DLL __declspec( dllexport )
#endif

namespace conct
{
	class ISimulatorDeviceContext;

	class ISimulatorContext
	{
	public:

		virtual ISimulatorDeviceContext*	registerDevice( uintreg deviceId ) = 0;
		virtual ISimulatorDeviceContext*	getDevice( uintreg deviceId ) = 0;
		virtual ISimulatorDeviceContext*	getCurrentDevice() = 0;
	};

	SIMULATOR_DLL ISimulatorContext& getSimulatorContext();
}