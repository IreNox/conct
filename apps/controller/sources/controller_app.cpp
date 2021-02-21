#include "controller_app.h"

#include <imapp/imapp.h>

namespace conct
{
	ControllerApp::ControllerApp()
	{

	}

	ControllerApp::~ControllerApp()
	{

	}

	void ControllerApp::setup()
	{
		m_device.setupDevice();
		m_controller.setup( &m_device.getRuntime() );

		m_config.load();
		m_ui.setup( &m_config );
	}

	void ControllerApp::doUI( const ImAppContext* pConext )
	{
		m_device.loopDevice();

		m_ui.doUI( pConext );
	}
}
