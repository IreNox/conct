#include "controller_app.h"

#include "conct_port_tcp_client.h"

#include <imapp/imapp.h>

namespace conct
{
	ControllerApp::ControllerApp()
	{

	}

	ControllerApp::~ControllerApp()
	{
		m_state.destroy();
	}

	void ControllerApp::setup()
	{
		m_device.setupDevice();

		m_config.load();

		m_state.setup( m_config, m_device.getRuntime() );
		m_ui.setup( &m_config );
	}

	void ControllerApp::doUI( ImAppContext* pConext )
	{
		m_device.loopDevice();

		m_state.loop();
		m_ui.doUI( pConext );
	}
}
