#include <stdio.h>

#include "controller_app.h"

#include <imapp/imapp.h>

void* __cdecl ImAppProgramInitialize( ImAppParameters* parameters, int argc, char* argv[] )
{
	parameters->windowTitle		= "Conct";
	parameters->tickIntervalMs	= 100u;

	conct::ControllerApp* conctApp = new conct::ControllerApp();
	if( !conctApp->setup() )
	{
		delete conctApp;
		return nullptr;
	}

	return conctApp;
}

void __cdecl ImAppProgramDoDefaultWindowUi( ImAppContext* imapp, void* programContext, ImUiSurface* surface )
{
	conct::ControllerApp* conctApp = (conct::ControllerApp*)programContext;
	conctApp->doUI( imapp, surface );
}

void __cdecl ImAppProgramShutdown( ImAppContext* pImAppContext, void* programContext )
{
	conct::ControllerApp* conctApp = (conct::ControllerApp*)programContext;
	delete conctApp;
}
