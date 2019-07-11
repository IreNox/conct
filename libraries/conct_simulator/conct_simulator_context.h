#pragma once

#include "conct_map.h"
#include "conct_mutex.h"
#include "conct_thread_local_storage.h"
#include "conct_simulator_rf24_context.h"

namespace conct
{
	class SimulatorDeviceContext;

	class SimulatorContext
	{
	public:

								SimulatorContext();
								~SimulatorContext();

		SimulatorDeviceContext*	registerDevice( uintreg deviceId );
		SimulatorDeviceContext*	getDevice( uintreg deviceId );
		SimulatorDeviceContext*	getCurrentDevice();

		SimulatorRf24Context&	getRf24Context() { return m_rf24; }

	private:

		typedef Map< uintreg, SimulatorDeviceContext* > DeviceMap;

		ThreadLocalStorageHandle	m_threadDevices;

		Mutex						m_mutex;
		DeviceMap					m_devices;

		SimulatorRf24Context		m_rf24;
	};
}