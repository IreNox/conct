#pragma once

#include "gen/device_interface.h"

#include "conct_controller.h"

namespace conct
{
	class DeviceAndroidController : public DeviceInterface
	{
	public:

		Controller&		getController() { return m_controller; }

	protected:

		virtual void	setup() CONCT_OVERRIDE_FINAL;
		virtual void	loop() CONCT_OVERRIDE_FINAL;

	private:

		Controller		m_controller;
	};
}
