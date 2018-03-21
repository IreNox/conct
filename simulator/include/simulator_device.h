#pragma once

#include "conct_core.h"

#include <vector>

namespace conct
{
	class Controller;

	struct SimulatorInstance
	{
		InstanceId				id;
		TypeCrc					type;
		std::vector< uint8 >	data;
	};

	struct SimulatorDeviceData
	{
		Controller*							pController;
		std::vector< SimulatorInstance >	instances;
	};

	class SimulatorDevice
	{
	public:

		virtual			~SimulatorDevice() { }

		virtual void	setup() = 0;
		virtual void	loop() = 0;

		virtual void 	fillData( SimulatorDeviceData& targetData ) = 0;
	};

	typedef SimulatorDevice* ( *CreateSimulatorFunction )( );
}
