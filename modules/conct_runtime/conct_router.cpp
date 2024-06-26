#include "conct_router.h"

#include "conct_runtime.h"

namespace conct
{
	Router::Router( Runtime& runtime )
		: m_runtime( runtime )
	{
	}

	ArrayView< const DeviceConnection > Router::getConnectedDevices() const
	{
		m_devices.clear();
		m_runtime.getDevices( m_devices );

		return m_devices;
	}

	void Router::ChangeDevice( DeviceId id, DeviceStatus status )
	{
		m_runtime.changeDevice( id, status );
	}
}
