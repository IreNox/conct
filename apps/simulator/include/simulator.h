#pragma  once

#include "conct_array.h"
#include "console.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <atomic>
#include <thread>

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

			std::atomic< bool >	running;
			std::thread			thread;
		};

		Array< Device >			m_devices;
		Console					m_console;

		bool					loadDevices();
		void					setupDevices();
		void					startDevices();
		void					freeDevices();

		static void				runDevice( Device* pDevice );
	};
}