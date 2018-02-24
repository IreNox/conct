#pragma  once

#include <vector>

namespace conct
{
	class SimulatorDevice;

	class Simulator
	{
	public:

		int run();

	private:

		std::vector< SimulatorDevice* >	m_devices;

		void							setupDevices();
	};
}