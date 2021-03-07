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

	bool ControllerApp::setup()
	{
		m_device.setupDevice();

		m_config.load();

		if( !m_state.setup( m_config, m_device.getRuntime() ) )
		{
			return false;
		}

		m_ui.setup( m_config, m_state );

		return true;
	}

	void ControllerApp::doUI( ImAppContext* pConext )
	{
		m_device.loopDevice();

		m_state.loop();
		m_ui.doUI( pConext );
	}
}
