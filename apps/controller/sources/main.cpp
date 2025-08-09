#include <stdio.h>

#include "controller_app.h"

#include <imapp/imapp.h>

void* __cdecl ImAppProgramInitialize( ImAppParameters* parameters, int argc, char* argv[] )
{
	parameters->tickIntervalMs		= 100u;
	parameters->resPath				= "./../../../assets";
	parameters->defaultResPakName	= "controller";
	parameters->windowTitle			= "conct controller";
	parameters->windowClearColor	= ImUiColorCreate( 0xf7, 0xf7, 0xf7, 0xff );

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
	ImAppResPakActivateTheme( imapp, ImAppResourceGetDefaultPak( imapp ), "theme" );

	conct::ControllerApp* conctApp = (conct::ControllerApp*)programContext;
	conctApp->doUI( imapp, surface );
}

void __cdecl ImAppProgramShutdown( ImAppContext* pImAppContext, void* programContext )
{
	conct::ControllerApp* conctApp = (conct::ControllerApp*)programContext;
	delete conctApp;
}
