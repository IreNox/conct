#pragma once

#include "conct_runtime.h"
#include "conct_vector.h"

namespace conct
{
	class RuntimeHigh;

	class Router
	{
	public:

											Router( RuntimeHigh& runtime );

		ArrayView< DeviceConnection >		getConnectedDevices() const;

		// interface functions
		void								ChangeDevice( DeviceId id, DeviceStatus status );

	private:

		RuntimeHigh&						m_runtime;

		mutable Vector< DeviceConnection >	m_devices; // TODO: get rid of mutable
	};
}