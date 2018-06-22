#pragma once

#include "conct_map.h"
#include "conct_thread_local_storage.h"

#define CONCT_IS_DLL CONCT_OFF
#include "i_simulator_context.h"

namespace conct
{
	class ISimulatorDeviceContext;

	class SimulatorContext : public ISimulatorContext
	{
	public:

											SimulatorContext();
											~SimulatorContext();

		virtual ISimulatorDeviceContext*	registerDevice( uintreg deviceId ) CONCT_OVERRIDE_FINAL;
		virtual ISimulatorDeviceContext*	getDevice( uintreg deviceId ) CONCT_OVERRIDE_FINAL;
		virtual ISimulatorDeviceContext*	getCurrentDevice() CONCT_OVERRIDE_FINAL;

	private:

		typedef Map< uintreg, ISimulatorDeviceContext* > DeviceMap;

		ThreadLocalStorageHandle	m_threadDevices;
		DeviceMap					m_devices;
	};
}