#pragma once

#include "conct_core.h"

#include <tiki/tiki_dynamic_string.h>

namespace conct
{
	class Controller;
	class Router;

	struct SimulatorInstance
	{
		InstanceId		id;
		TypeCrc			type;
		DynamicString	data;
	};

	struct SimulatorDeviceData
	{
		using InstanceArray = DynamicArray< SimulatorInstance >;

		DynamicString	name;

		Router*			pRouter;
		Controller*		pController;
		InstanceArray	instances;
	};

	class ISimulatorDevice
	{
	public:

		virtual			~ISimulatorDevice() { }

		virtual void	setup() = 0;
		virtual void	loop() = 0;

		virtual void 	fillData( SimulatorDeviceData& targetData ) = 0;
	};

	typedef ISimulatorDevice* ( *CreateSimulatorFunction )( );
}
