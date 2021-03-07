#pragma once

#include "conct_device.h"
#include "conct_runtime_high.h"

#include "device_controller.h"

#include "controller_config.h"
#include "controller_state.h"
#include "controller_ui.h"

struct ImAppContext;

namespace conct
{
	class PortTcpClient;

	class ControllerApp
	{
	public:

							ControllerApp();
							~ControllerApp();

		void				setup();

		void				doUI( ImAppContext* pConext );

	private:

		DeviceController	m_device;

		ControllerConfig	m_config;
		ControllerState		m_state;
		ControllerUI		m_ui;
	};
}