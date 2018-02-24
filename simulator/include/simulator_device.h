#pragma once

namespace conct
{
	class SimulatorDevice
	{
	public:

		virtual void setup() = 0;
		virtual void loop() = 0;
	};
}
