#include "conct_router.h"

#include "conct_runtime_high.h"

namespace conct
{
	Router::Router( RuntimeHigh& runtime )
		: m_runtime( runtime )
	{
	}

	ArrayView< DeviceConnection > Router::getConnectedDevices() const
	{
		m_devices.clear();
		m_runtime.getDevices( m_devices );

		return m_devices.toView();
	}

	void Router::ChangeDevice( DeviceId id, DeviceStatus status )
	{
		m_runtime.changeDevice( id, status );
	}
}
