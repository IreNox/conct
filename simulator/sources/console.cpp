#include "console.h"

#include <windows.h>

namespace conct
{
	Console::Console()
	{
		m_changeFlags		= ChangeFlag_None;
		m_selectedDevice	= 0u;
		m_deviceMode		= DeviceMode_Invalid;

		const HANDLE consoleHandle = GetStdHandle( STD_INPUT_HANDLE );
		DWORD modes;
		GetConsoleMode( consoleHandle, &modes );
		SetConsoleMode( consoleHandle, modes | ENABLE_WINDOW_INPUT );
	}

	void Console::addDevice( SimulatorDevice* pDevice )
	{
		Device device;
		device.pDevice = pDevice;
		pDevice->fillData( device.data );

		m_devices.push_back( device );

		m_changeFlags |= ChangeFlag_DeviceList;

		selectDevice( m_devices.size() - 1u );
	}

	void Console::removeDevice( SimulatorDevice* pDevice )
	{
		for( DeviceVector::iterator it = m_devices.begin(); it != m_devices.end(); ++it )
		{
			if( it->pDevice == pDevice )
			{
				it = m_devices.erase( it );
			}
		}

		m_changeFlags |= ChangeFlag_DeviceList;
	}

	void Console::update()
	{
		if( getKeyState( VK_ESCAPE ) )
		{
			exit( 0u );
		}

		updateDeviceSelection();
		updateModeSelection();

		if( m_changeFlags.isSet( ChangeFlag_DeviceList ) )
		{
			drawDeviceList();
		}

		if( m_changeFlags.isSet( ChangeFlag_Device ) )
		{
			drawDeviceModes();
		}

		if( m_selectedDevice < m_devices.size() )
		{
			Device& device = m_devices[ m_selectedDevice ];
			device.pDevice->fillData( device.data );

			switch( m_deviceMode )
			{
			case DeviceMode_Controller:
				drawDeviceController();
				break;

			case DeviceMode_Instances:
				drawDeviceInstances();
				break;

			default:
				break;
			}
		}
	}

	void Console::updateDeviceSelection()
	{
		for( size_t i = 0u; i < m_devices.size(); ++i )
		{
			if( getKeyState( int( VK_F1 + i ) ) )
			{
				selectDevice( i );
			}
		}
	}

	void Console::updateModeSelection()
	{
		for( size_t i = 0u; i < m_enabledModes.size(); ++i )
		{
			if( getKeyState( int( '1' + i ) ) )
			{
				m_deviceMode = m_enabledModes[ i ];
				m_changeFlags |= ChangeFlag_Device;
			}
		}
	}

	void Console::selectDevice( size_t index )
	{
		m_selectedDevice = index;
		m_deviceMode = DeviceMode_Invalid;
		m_enabledModes.clear();

		const Device& selectedDevice = m_devices[ m_selectedDevice ];
		if( selectedDevice.data.pController != nullptr )
		{
			m_enabledModes.push_back( DeviceMode_Controller );
		}

		if( !selectedDevice.data.instances.empty() )
		{
			m_enabledModes.push_back( DeviceMode_Instances );
		}

		if( !m_enabledModes.empty() )
		{
			m_deviceMode = m_enabledModes[ 0u ];
		}

		m_changeFlags |= ChangeFlag_DeviceList;
		m_changeFlags |= ChangeFlag_Device;
	}

	void Console::drawDeviceList()
	{
		drawFillRectangle( 0u, 0u, getSize().x, 3u, ' ' );

		uint16 x = 0u;
		size_t i = 0u;
		for( const Device& device : m_devices )
		{
			x += drawButton( x, 0u, device.data.name.c_str(), i == m_selectedDevice ? LineType_Double : LineType_Single ) + 1u;
			i++;
		}

		m_changeFlags.unset( ChangeFlag_DeviceList );
	}

	void Console::drawDeviceModes()
	{
		const uint16x2 size = getSize();
		drawFillRectangle( 0u, 3u, size.x, size.y - 3u, ' ' );

		const Device& selectedDevice = m_devices[ m_selectedDevice ];

		uint16 x = 0u;
		if( selectedDevice.data.pController != nullptr )
		{
			x += drawButton( x, 3u, "Controller", m_deviceMode == DeviceMode_Controller ? LineType_Double : LineType_Single ) + 1u;
		}

		if( !selectedDevice.data.instances.empty() )
		{
			x += drawButton( x, 3u, "Instances", m_deviceMode == DeviceMode_Instances ? LineType_Double : LineType_Single ) + 1u;
		}

		m_changeFlags.unset( ChangeFlag_Device );
	}

	void Console::drawDeviceController()
	{
		uint16 width = measureTextSize( "Instances:" ).x;
		for( const ControllerInstance& instance : m_controllerInstances )
		{
			width = CONCT_MAX( measureTextSize( instance.name.c_str() ).x, width );
		}
		width += 4u;

		const uint16x2 size = getSize();
		const uint16 left = size.x - width + 2u;

		drawRectangle( 0u, 6u, size.x - width, size.y - 9u, LineType_Single );
		drawRectangle( 0u, size.y - 3u, size.x - width, 3u, LineType_Single );
		drawRectangle( size.x - width, 6u, width, size.y - 6u, LineType_Single );

		uint16 top = 7u;
		drawText( left, top++, "Instances:" );
		for( const ControllerInstance& instance : m_controllerInstances )
		{
			drawText( left, top++, instance.name.c_str() );
		}

		drawText( 2u, size.y - 2u, m_controllerCommand.c_str() );

		const HANDLE consoleHandle = GetStdHandle( STD_INPUT_HANDLE );
		INPUT_RECORD input;
		DWORD read;
		PeekConsoleInputA( consoleHandle, &input, 1u, &read );
		if( read > 0u )
		{
			if( input.EventType == KEY_EVENT && input.Event.KeyEvent.bKeyDown )
			{
				const char c = input.Event.KeyEvent.uChar.AsciiChar;
				if( c == '\r' )
				{

				}
				else if( c == '\0' )
				{

				}
				else if( c == '\b' )
				{
					if( !m_controllerCommand.empty() )
					{
						m_controllerCommand.pop_back();
						drawCharacter( uint16( 2u + m_controllerCommand.size() ), size.y - 2u, ' ' );
					}
				}
				else
				{
					m_controllerCommand += c;
				}
			}

			ReadConsoleInputA( consoleHandle, &input, 1u, &read );
		}
	}

	void Console::drawDeviceInstances()
	{
		const Device& selectedDevice = m_devices[ m_selectedDevice ];

		uint16 x = 0u;
		uint16 y = 6u;
		for( const SimulatorInstance& instance : selectedDevice.data.instances )
		{
			const uint16x2 size = drawTextMultiline( x + 2u, y + 1u, instance.data.c_str() );
			drawRectangle( x, y, size.x + 4u, size.y + 2u, LineType_Single );

			x += size.x + 4u;
		}
	}

	uint16x2 Console::measureTextSize( const char* pString )
	{
		const uint16 length = uint16( strlen( pString ) );

		uint16 x = 0u;
		uint16x2 size = { 0u, 0u };
		for( uint16 i = 0u; i < length; ++i )
		{
			const char c = pString[ i ];
			if( c == '\n' )
			{
				size.x = CONCT_MAX( size.x, x );
				size.y++;

				x = 0u;

				continue;
			}

			x++;
		}

		size.x = CONCT_MAX( size.x, x );
		return size;
	}

	uint16 Console::drawButton( uint16 x, uint16 y, const char* pText, LineType type )
	{
		const uint16 width = uint16( strlen( pText ) + 4u );

		drawRectangle( x, y, width, 3u, type );
		drawText( x + 2u, y + 1u, pText );

		return width;
	}

	void Console::drawRectangle( uint16 x, uint16 y, uint16 width, uint16 height, LineType type )
	{
		static const wchar_t* s_apChars[] =
		{
			L"┌┐└┘─│",
			L"╔╗╚╝═║"
		};
		const wchar_t* pType = s_apChars[ type ];

		drawCharacter( x, y, pType[ 0u ] );
		drawCharacter( x + width - 1u, y, pType[ 1u ] );
		drawCharacter( x, y + height - 1u, pType[ 2u ] );
		drawCharacter( x + width - 1u, y + height - 1u, pType[ 3u ] );

		if( width > 2u )
		{
			drawCharacterRepeated( x + 1u, y, pType[ 4u ], width - 2u );
			drawCharacterRepeated( x + 1u, y + height - 1u, pType[ 4u ], width - 2u );
		}

		for( uint i = 0u; i < height - 2u; ++i )
		{
			drawCharacter( x, y + i + 1u, pType[ 5u ] );
			drawCharacter( x + width - 1u, y + i + 1u, pType[ 5u ] );
		}
	}

	void Console::drawFillRectangle( uint16 x, uint16 y, uint16 width, uint16 height, wchar_t c )
	{
		for( uint i = 0u; i < height; ++i )
		{
			drawCharacterRepeated( x, y + i, c, width );
		}
	}

	void Console::drawText( uint16 x, uint16 y, const char* pString )
	{
		const HANDLE consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

		COORD coord;
		coord.X	= x;
		coord.Y	= y;

		DWORD written = 0u;
		WriteConsoleOutputCharacterA( consoleHandle, pString, (DWORD)strlen( pString ), coord, &written );
	}

	uint16x2 Console::drawTextMultiline( uint16 x, uint16 y, const char* pString )
	{
		const uint16 baseX = x;
		const uint16 length = uint16( strlen( pString ) );

		uint16x2 size = { 0u, 0u };
		for( uint16 i = 0u; i < length; ++i )
		{
			const char c = pString[ i ];
			if( c == '\n' )
			{
				size.x = CONCT_MAX( size.x, x - baseX );
				size.y++;

				x = baseX;
				continue;
			}

			drawCharacter( x, y + size.y, c );
			x++;
		}

		size.x = CONCT_MAX( size.x, x - baseX );
		return size;
	}

	void Console::drawCharacter( uint16 x, uint16 y, wchar_t c )
	{
		drawCharacterRepeated( x, y, c, 1u );
	}

	void Console::drawCharacterRepeated( uint16 x, uint16 y, wchar_t c, uint16 count )
	{
		const HANDLE consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

		COORD coord;
		coord.X	= x;
		coord.Y	= y;

		DWORD written = 0u;
		FillConsoleOutputCharacterW( consoleHandle, c, count, coord, &written );
	}

	uint16x2 Console::getSize()
	{
		const HANDLE consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

		CONSOLE_SCREEN_BUFFER_INFO info;
		GetConsoleScreenBufferInfo( consoleHandle, &info );

		const uint16x2 size = {
			uint16( info.srWindow.Right - info.srWindow.Left ),
			uint16( info.srWindow.Bottom - info.srWindow.Top )
		};
		return size;
	}

	bool Console::getKeyState( int code )
	{
		return GetAsyncKeyState( code );
	}
}