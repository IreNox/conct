#pragma once

#include "conct_flags.h"
#include "conct_structs.h"
#include "conct_runtime.h"

#include "console_plugin.h"

namespace conct
{
	class ConsolePlugin;
	class SimulatorDevice;

	class Console
	{
	public:

				Console();

		void	addDevice( SimulatorDevice* pDevice );
		void	removeDevice( SimulatorDevice* pDevice );

		void	update();

	private:

		enum ChangeFlag
		{
			ChangeFlag_None			= 0u,

			ChangeFlag_DeviceList	= 1u << 0u,
			ChangeFlag_Device		= 1u << 1u,
			ChangeFlag_Instance		= 1u << 2u,
			ChangeFlag_CommandLine	= 1u << 3u,
		};

		typedef std::vector< ConsoleDevice > DeviceVector;

		Flags8< ChangeFlag >		m_changeFlags;

		size_t						m_selectedDevice;
		DeviceVector				m_devices;
		size_t						m_pluginIndex;

		void						updateDeviceSelection();
		void						updateModeSelection( ConsoleDevice* pDevice );

		void						selectDevice( size_t index );

		void						drawDeviceList();
		void						drawDeviceModes( const ConsoleDevice* pDevice );
	};
}