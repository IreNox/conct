#include <stdio.h>

#include "controller_app.h"

#include <imapp/imapp.h>

void* __cdecl ImAppProgramInitialize( ImAppParameters* pParameters )
{
	pParameters->windowTitle		= "Conct";
	pParameters->defaultFullWindow	= false;
	pParameters->tickIntervalMs		= 100u;

	conct::ControllerApp* pApp = new conct::ControllerApp();
	if( !pApp->setup() )
	{
		delete pApp;
		return nullptr;
	}

	return pApp;
}

void __cdecl ImAppProgramDoUi( ImAppContext* pImAppContext, void* pProgramContext )
{
	conct::ControllerApp* pApp = (conct::ControllerApp*)pProgramContext;
	pApp->doUI( pImAppContext );
}

void __cdecl ImAppProgramShutdown( ImAppContext* pImAppContext, void* pProgramContext )
{
	conct::ControllerApp* pApp = (conct::ControllerApp*)pProgramContext;
	delete pApp;
}
