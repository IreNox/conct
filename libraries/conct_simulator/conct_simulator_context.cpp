#include "conct_simulator_context.h"

#include "conct_simulator_device_context.h"

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

	SimulatorDeviceContext* SimulatorContext::registerDevice( uintreg deviceId )
	{
		SimulatorDeviceContext* pDevice = new SimulatorDeviceContext( deviceId );

		{
			MutexLock lock( m_mutex );
			m_devices.insert( deviceId, pDevice );
		}
		thread_local_storage::setValue( m_threadDevices, (uintptr)pDevice );

		return pDevice;
	}

	SimulatorDeviceContext* SimulatorContext::getDevice( uintreg deviceId )
	{
		MutexLock lock( m_mutex );
		return m_devices[ deviceId ];
	}

	SimulatorDeviceContext* SimulatorContext::getCurrentDevice()
	{
		return ( SimulatorDeviceContext* )thread_local_storage::getValue( m_threadDevices );
	}
}