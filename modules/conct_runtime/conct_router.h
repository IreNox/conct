#pragma once

#include "conct_runtime.h"

#include <tiki/tiki_dynamic_array.h>

namespace conct
{
	class Runtime;

	class Router
	{
	public:

											Router( Runtime& runtime );

		ArrayView< const DeviceConnection >	getConnectedDevices() const;

		// interface functions
		void								ChangeDevice( DeviceId id, DeviceStatus status );

	private:

		using DeviceConnectionArray = DynamicArray< DeviceConnection >;

		Runtime&							m_runtime;

		mutable DeviceConnectionArray		m_devices; // TODO: get rid of mutable
	};
}