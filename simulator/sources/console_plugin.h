#pragma once

#include "simulator_device.h"

#include <vector>

namespace conct
{
	class ConsolePlugin;

	struct ConsoleDevice
	{
		typedef std::vector< ConsolePlugin* > PluginVector;

		SimulatorDevice*	pDevice;
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