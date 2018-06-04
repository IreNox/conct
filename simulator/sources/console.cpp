#include "console.h"

#include "base.h"

#include "console_controller.h"
#include "console_input.h"
#include "console_instances.h"
#include "console_plugin.h"
#include "console_render.h"

#include <iostream>

namespace conct
{
	Console::Console()
	{
		m_changeFlags		= ChangeFlag_None;
		m_selectedDevice	= 0u;
		m_pluginIndex		= 0u;

		ConsoleInput::setup();
		ConsoleRenderer::setup();

		std::filesystem::path exePath = getExecutableName();
		std::filesystem::path basePath = exePath.parent_path().parent_path().parent_path().parent_path();
		std::filesystem::path configPath = basePath;
		configPath.append( "config" );

		std::filesystem::path typesPath = configPath;
		typesPath.append( "types" );
		if( !m_types.load( typesPath.generic_string() ) )
		{
			std::cout << "could not load types" << std::endl;
			exit( 0u );
		}
	}

	void Console::addDevice( SimulatorDevice* pDevice )
	{
		ConsoleDevice device;
		device.pDevice = pDevice;
		pDevice->fillData( device.data );

		if( device.data.pController != nullptr )
		{
			device.plugins.push_back( new ConsoleController( &m_types ) );
		}

		if( !device.data.instances.empty() )
		{
			device.plugins.push_back( new ConsoleInstances() );
		}

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
				for( ConsolePlugin* pPlugin : it->plugins )
				{
					delete pPlugin;
				}

				it = m_devices.erase( it );
			}
		}

		m_changeFlags |= ChangeFlag_DeviceList;
	}

	void Console::update()
	{
		if( ConsoleInput::ConsoleInput::getKeyState( ConsoleKey_Escape ) )
		{
			exit( 0u );
		}

		ConsoleInput::readKey( [ this ]( ConsoleKey key )
		{
			if( key >= ConsoleKey_F1 && key <= ConsoleKey_F4 )
			{
				const uintreg index = key - ConsoleKey_F1;
				if( index < m_devices.size() )
				{
					selectDevice( index );
					return true;
				}
			}

			return false;
		} );

		ConsoleDevice* pDevice = nullptr;
		if( m_selectedDevice < m_devices.size() )
		{
			pDevice = &m_devices[ m_selectedDevice ];
		}

		ConsoleInput::readKey( [ this, pDevice ]( ConsoleKey key )
		{
			if( pDevice == nullptr )
			{
				return false;
			}

			if( key >= ConsoleKey_F5 && key <= ConsoleKey_F8 )
			{
				const uintreg index = key - ConsoleKey_F5;
				if( index < pDevice->plugins.size() )
				{
					if( m_pluginIndex < pDevice->plugins.size() )
					{
						pDevice->plugins[ m_pluginIndex ]->deactivate( *pDevice );
					}

					m_pluginIndex = index;
					pDevice->plugins[ m_pluginIndex ]->activate( *pDevice );

					m_changeFlags |= ChangeFlag_Device;
					return true;
				}
			}

			return false;
		} );

		if( m_changeFlags.isSet( ChangeFlag_DeviceList ) )
		{
			drawDeviceList();
		}

		if( m_changeFlags.isSet( ChangeFlag_Device ) )
		{
			drawDeviceModes( pDevice );
		}

		if( pDevice != nullptr )
		{
			pDevice->pDevice->fillData( pDevice->data );

			if( m_pluginIndex < pDevice->plugins.size() )
			{
				ConsolePlugin* pPlugin = pDevice->plugins[ m_pluginIndex ];
				pPlugin->update( *pDevice );
				pPlugin->draw( *pDevice );
			}
		}
	}

	void Console::selectDevice( size_t index )
	{
		if( m_selectedDevice < m_devices.size() )
		{
			ConsoleDevice& device = m_devices[ m_selectedDevice ];
			if( m_pluginIndex < device.plugins.size() )
			{
				device.plugins[ m_pluginIndex ]->deactivate( device );
			}
		}

		m_selectedDevice = index;
		m_pluginIndex = 0u;

		ConsoleDevice& device = m_devices[ m_selectedDevice ];
		if( m_pluginIndex < device.plugins.size() )
		{
			device.plugins[ m_pluginIndex ]->activate( device );
		}

		m_changeFlags |= ChangeFlag_DeviceList;
		m_changeFlags |= ChangeFlag_Device;
	}

	void Console::drawDeviceList()
	{
		ConsoleRenderer::drawFillRectangle( 0u, 0u, ConsoleRenderer::getSize().x, 3u, ' ' );

		uint16 x = 0u;
		size_t i = 0u;
		for( const ConsoleDevice& device : m_devices )
		{
			x += ConsoleRenderer::drawButton( x, 0u, device.data.name.c_str(), i == m_selectedDevice ? LineType_Double : LineType_Single ) + 1u;
			i++;
		}

		m_changeFlags.unset( ChangeFlag_DeviceList );
	}

	void Console::drawDeviceModes( const ConsoleDevice* pDevice )
	{
		const uint16x2 size = ConsoleRenderer::getSize();
		ConsoleRenderer::drawFillRectangle( 0u, 3u, size.x, 3u, ' ' );

		if( pDevice == nullptr )
		{
			return;
		}

		uint16 x = 0u;
		size_t i = 0u;
		for( ConsolePlugin* pPlugin : pDevice->plugins )
		{
			x += ConsoleRenderer::drawButton( x, 3u, pPlugin->getName(), i == m_pluginIndex ? LineType_Double : LineType_Single ) + 1u;
			i++;
		}

		m_changeFlags.unset( ChangeFlag_Device );
	}
}