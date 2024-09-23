#pragma once

#include "conct_mutex.h"
#include "conct_thread_local_storage.h"

#include <tiki/tiki_hash_map.h>

namespace conct
{
	class DynamicMemory;
	class SimulatorDeviceContext;

	class SimulatorContext
	{
	public:

								SimulatorContext();
								~SimulatorContext();

		DynamicMemory*			getDynamicMemory();

		SimulatorDeviceContext*	registerDevice( uintreg deviceId );
		SimulatorDeviceContext*	getDevice( uintreg deviceId );
		SimulatorDeviceContext*	getCurrentDevice();

	private:

		typedef HashMap< uintreg, SimulatorDeviceContext* > DeviceMap;

		ThreadLocalStorageHandle	m_threadDevices;

		DynamicMemory*				m_pMemory;

		Mutex						m_mutex;
		DeviceMap					m_devices;
	};
}