#pragma once

#include "conct_vector.h"

namespace conct
{
	class RuntimeHigh;

	class Router
	{
	public:

									Router( const RuntimeHigh& runtime );

		ArrayView< DeviceId >		getConnectedDevices() const;

	private:

		const RuntimeHigh&			m_runtime;

		mutable Vector< DeviceId >	m_devices; // TODO: get rid of mutable
	};
}