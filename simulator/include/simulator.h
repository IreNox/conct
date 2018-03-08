#pragma  once

#include <windows.h>

#include <vector>

namespace conct
{
	class SimulatorDevice;

	class Simulator
	{
	public:

		int run();

	private:

		struct Device
		{
			HMODULE				dllHandle;
			SimulatorDevice*	pDevice;
		};

		std::vector< Device >	m_devices;

		bool					loadDevices();
		void					setupDevices();
		void					loopDevices();
		void					freeDevices();
	};
}