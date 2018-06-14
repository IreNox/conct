#include "conct_router.h"

#include "conct_runtime_high.h"

namespace conct
{
	Router::Router( const RuntimeHigh& runtime )
		: m_runtime( runtime )
	{
	}

	ArrayView< DeviceId > Router::getConnectedDevices() const
	{
		m_devices.clear();
		m_runtime.getDevices( m_devices );

		return m_devices.toArrayView();
	}
}
