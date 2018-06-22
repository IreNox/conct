#pragma once

#include "conct_core.h"
#include "conct_dynamic_string.h"
#include "conct_vector.h"

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
		DynamicString				name;

		Router*						pRouter;
		Controller*					pController;
		Vector< SimulatorInstance >	instances;
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
