#include "console.h"

#include "console_controller.h"
#include "console_input.h"
#include "console_instances.h"
#include "console_plugin.h"
#include "console_render.h"
#include "conct_path.h"

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

		const Path exePath = Path::getExecutablePath();
		const Path basePath = exePath.getParent().getParent().getParent().getParent().getParent();
		const Path configPath = basePath.push( "config"_s );
		const Path typesPath = configPath.push( "types"_s );

		if( !m_types.load( typesPath ) )
		{
			std::cout << "could not load types" << std::endl;
			exit( 0u );
		}
	}

	void Console::addDevice( ISimulatorDevice* pDevice )
	{
		ConsoleDevice device;
		device.pDevice = pDevice;
		pDevice->fillData( device.data );

		if( device.data.pController != nullptr )
		{
			device.plugins.pushBack( new ConsoleController( &m_types ) );
		}

		if( !device.data.instances.isEmpty() )
		{
			device.plugins.pushBack( new ConsoleInstances() );
		}

		m_devices.pushBack( device );

		m_changeFlags |= ChangeFlag_DeviceList;

		selectDevice( m_devices.getLength() - 1u );
	}

	void Console::removeDevice( ISimulatorDevice* pDevice )
	{
		for( ConsoleDevice& device : m_devices )
		{
			if( device.pDevice != pDevice )
			{
				continue;
			}

			for( ConsolePlugin* pPlugin : device.plugins )
			{
				delete pPlugin;
			}

			m_devices.eraseSorted( device );
			break;
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
				if( index < m_devices.getLength() )
				{
					selectDevice( index );
					return true;
				}
			}

			return false;
		} );

		ConsoleDevice* pDevice = nullptr;
		if( m_selectedDevice < m_devices.getLength() )
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
				if( index < pDevice->plugins.getLength() )
				{
					if( m_pluginIndex < pDevice->plugins.getLength() )
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

			if( m_pluginIndex < pDevice->plugins.getLength() )
			{
				ConsolePlugin* pPlugin = pDevice->plugins[ m_pluginIndex ];
				pPlugin->update( *pDevice );
				pPlugin->draw( *pDevice );
			}
		}
	}

	void Console::selectDevice( size_t index )
	{
		if( m_selectedDevice < m_devices.getLength() )
		{
			ConsoleDevice& device = m_devices[ m_selectedDevice ];
			if( m_pluginIndex < device.plugins.getLength() )
			{
				device.plugins[ m_pluginIndex ]->deactivate( device );
			}
		}

		m_selectedDevice = index;
		m_pluginIndex = 0u;

		ConsoleDevice& device = m_devices[ m_selectedDevice ];
		if( m_pluginIndex < device.plugins.getLength() )
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
			x += ConsoleRenderer::drawButton( x, 0u, device.data.name.toConstCharPointer(), i == m_selectedDevice ? LineType_Double : LineType_Single ) + 1u;
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