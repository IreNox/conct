#pragma once

#include "gen/device_interface.h"

#include "conct_controller.h"

namespace conct
{
	class PortTcpClient;

	class DeviceController : public DeviceInterface
	{
	public:

		PortTcpClient*				addPort( const char* pHostname, uint16 hostPort );
		void						removePort( PortTcpClient* pPort );

		RuntimeHigh&				getRuntime() { return m_runtime; }
		Controller&					getController() { return m_controller; }

	protected:

		virtual void				setup() CONCT_OVERRIDE_FINAL;
		virtual void				loop() CONCT_OVERRIDE_FINAL;

	private:

		Controller					m_controller;
	};
}
