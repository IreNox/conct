#pragma once

#include <tiki/tiki_dynamic_array.h>

#include "i_simulator_device.h"

namespace conct
{
	class ConsolePlugin;

	struct ConsoleDevice
	{
		typedef DynamicArray< ConsolePlugin* > PluginVector;

		ISimulatorDevice*	pDevice;
		SimulatorDeviceData	data;

		PluginVector		plugins;
	};

	class ConsolePlugin
	{
	public:

		virtual				~ConsolePlugin() { }

		virtual void		activate( ConsoleDevice& device ) = 0;
		virtual void		deactivate( ConsoleDevice& device ) = 0;

		virtual void		update( ConsoleDevice& device ) = 0;
		virtual void		draw( const ConsoleDevice& device ) const = 0;

		virtual const char*	getName() const = 0;
	};
}