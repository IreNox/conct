#pragma once

#include "conct_flags.h"
#include "conct_runtime.h"
#include "conct_structs.h"
#include "conct_type_collection.h"

#include "console_plugin.h"

namespace conct
{
	class ConsolePlugin;
	class ISimulatorDevice;

	class Console
	{
	public:

				Console();

		void	addDevice( ISimulatorDevice* pDevice );
		void	removeDevice( ISimulatorDevice* pDevice );

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

		typedef Vector< ConsoleDevice > DeviceVector;

		Flags8< ChangeFlag >		m_changeFlags;

		size_t						m_selectedDevice;
		DeviceVector				m_devices;
		size_t						m_pluginIndex;

		TypeCollection				m_types;

		void						selectDevice( size_t index );

		void						drawDeviceList();
		void						drawDeviceModes( const ConsoleDevice* pDevice );
	};
}