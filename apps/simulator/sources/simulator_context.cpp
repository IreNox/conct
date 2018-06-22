#include "simulator_context.h"

#include "simulator_device_context.h"

namespace conct
{
	SimulatorContext::SimulatorContext()
	{
		m_threadDevices = thread_local_storage::create();
	}

	SimulatorContext::~SimulatorContext()
	{
		for( const DeviceMap::PairType& pair : m_devices )
		{
			delete pair.value;
		}

		thread_local_storage::destroy( m_threadDevices );
	}

	ISimulatorDeviceContext* SimulatorContext::registerDevice( uintreg deviceId )
	{
		SimulatorDeviceContext* pDevice = new SimulatorDeviceContext( deviceId );

		{
			MutexLock lock( m_mutex );
			m_devices.insert( deviceId, pDevice );
		}
		thread_local_storage::setValue( m_threadDevices, (uintptr)pDevice );

		return pDevice;
	}

	ISimulatorDeviceContext* SimulatorContext::getDevice( uintreg deviceId )
	{
		MutexLock lock( m_mutex );
		return m_devices[ deviceId ];
	}

	ISimulatorDeviceContext* SimulatorContext::getCurrentDevice()
	{
		return ( ISimulatorDeviceContext* )thread_local_storage::getValue( m_threadDevices );
	}

	SIMULATOR_DLL ISimulatorContext& getSimulatorContext()
	{
		static SimulatorContext s_context;
		return s_context;
	}
}