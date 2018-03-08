#pragma once

namespace conct
{
	class SimulatorDevice
	{
	public:

		virtual ~SimulatorDevice() { }

		virtual void setup() = 0;
		virtual void loop() = 0;
	};

	typedef SimulatorDevice* ( *CreateSimulatorFunction )( );
}
