#pragma once

#include "conct_controller.h"
#include "conct_device.h"
#include "conct_runtime_high.h"
#include "conct_vector.h"

#include "device_controller.h"

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

		void				doUI( const ImAppContext* pConext );

	private:

		using PortVector = Vector< PortTcpClient* >;

		DeviceController	m_device;
		Controller			m_controller;

		PortVector			m_ports;
	};
}