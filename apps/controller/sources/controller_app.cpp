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
	}

	void ControllerApp::doUI( const ImAppContext* pConext )
	{
		m_device.loopDevice();
	}
}
