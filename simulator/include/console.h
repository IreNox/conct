#pragma once

#include "conct_flags.h"
#include "conct_structs.h"
#include "conct_runtime.h"

#include "simulator_device.h"

namespace conct
{
	class Console
	{
	public:

				Console();

		void	addDevice( SimulatorDevice* pDevice );
		void	removeDevice( SimulatorDevice* pDevice );

		void	update();

	private:

		enum LineType
		{
			LineType_Single,
			LineType_Double,

			LineType_Count
		};

		enum ChangeFlag
		{
			ChangeFlag_None			= 0u,

			ChangeFlag_DeviceList	= 1u << 0u,
			ChangeFlag_Device		= 1u << 1u,
			ChangeFlag_Instance		= 1u << 2u,
			ChangeFlag_CommandLine	= 1u << 3u,
		};

		enum DeviceMode
		{
			DeviceMode_Invalid = -1,

			DeviceMode_Controller,
			DeviceMode_Instances,

			DeviceMode_Count
		};

		struct Device
		{
			SimulatorDevice*	pDevice;
			SimulatorDeviceData	data;
		};

		struct ControllerInstance
		{
			std::string		name;
			TypeCrc			type;
			RemoteInstance	instance;
		};

		typedef std::vector< Device > DeviceVector;
		typedef std::vector< DeviceMode > DeviceModeVector;
		typedef std::vector< ControllerInstance > ControllerInstanceVector;

		Flags8< ChangeFlag >		m_changeFlags;

		size_t						m_selectedDevice;
		DeviceVector				m_devices;
		DeviceMode					m_deviceMode;
		DeviceModeVector			m_enabledModes;

		std::string					m_controllerCommand;
		ControllerInstanceVector	m_controllerInstances;

		void						updateDeviceSelection();
		void						updateModeSelection();

		void						selectDevice( size_t index );

		void						drawDeviceList();
		void						drawDeviceModes();
		void						drawDeviceController();
		void						drawDeviceInstances();

		uint16x2					measureTextSize( const char* pString );

		uint16						drawButton( uint16 x, uint16 y, const char* pText, LineType type );
		void						drawRectangle( uint16 x, uint16 y, uint16 width, uint16 height, LineType type );
		void						drawFillRectangle( uint16 x, uint16 y, uint16 width, uint16 height, wchar_t c );
		void						drawText( uint16 x, uint16 y, const char* pString );
		uint16x2					drawTextMultiline( uint16 x, uint16 y, const char* pString );
		void						drawCharacter( uint16 x, uint16 y, wchar_t c );
		void						drawCharacterRepeated( uint16 x, uint16 y, wchar_t c, uint16 count );

		uint16x2					getSize();

		bool						getKeyState( int code );
	};
}