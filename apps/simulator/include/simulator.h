#pragma  once

#include "conct_atomic.h"

#include "console.h"

#include <tiki/tiki_array_view.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <thread>

namespace conct
{
	class ISimulatorDevice;

	class Simulator
	{
	public:

		int						run();

	private:

		struct Device
		{
			uintreg				deviceId;

			HMODULE				dllHandle;
			ISimulatorDevice*	pDevice;

			Atomic< bool >		running;
			std::thread			thread;
		};

		ArrayView< Device >		m_devices;
		Console					m_console;

		bool					loadDevices();
		void					setupDevices();
		void					startDevices();
		void					freeDevices();

		static void				runDevice( Device* pDevice );
	};
}