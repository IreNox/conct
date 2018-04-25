#include "console_controller.h"

#include "type_collection.h"
#include "interface_type.h"

#include "conct_controller.h"

#include "console_input.h"
#include "console_render.h"

#include <sstream>

namespace conct
{
	/*static*/ const ConsoleController::CommandInfo ConsoleController::s_aCommands[] =
	{
		{ "help",			&ConsoleController::executeHelpCommand },
		{ "getInstance",	&ConsoleController::executeGetInstanceCommand },
	};

	ConsoleController::ConsoleController( TypeCollection* pTypes )
		: m_pTypes( pTypes )
		, m_instancesWidth( 0u )
	{
		m_commandText = "getInstance 1 Home.Light";

		updateInstancesWidth();
	}

	void ConsoleController::activate( ConsoleDevice& device )
	{
		m_drawInstances	= true;
		m_drawLog		= true;
		m_drawCommand	= true;

		draw( device );
	}

	void ConsoleController::deactivate( ConsoleDevice& device )
	{
	}

	void ConsoleController::update( ConsoleDevice& device )
	{
		m_drawInstances		= false;
		m_drawLog			= false;
		m_drawCommand		= false;

		char c;
		if( ConsoleInput::readChar( c ) )
		{
			m_commandText += c;
			m_drawCommand = true;
		}

		ConsoleKey key;
		if( ConsoleInput::readKey( key ) )
		{
			switch( key )
			{
			case ConsoleKey_Return:
				executeCommand( device );
				break;

			case ConsoleKey_Backspace:
				if( !m_commandText.empty() )
				{
					m_commandText.pop_back();
					m_drawCommand = true;
				}
				break;

			case ConsoleKey_Tab:
				autoComplete( device );
				break;

			case ConsoleKey_Up:
				if( !m_commandHistory.empty() )
				{
					m_commandText = m_commandHistory.back();
					m_drawCommand = true;
				}
				break;

			case ConsoleKey_Down:
				break;

			case ConsoleKey_Left:
				break;

			case ConsoleKey_Right:
				break;

			default:
				break;
			}
		}
	}

	void ConsoleController::draw( const ConsoleDevice& device ) const
	{
		const uint16x2 size = ConsoleRenderer::getSize();

		drawInstances( size );
		drawLog( size );
		drawCommand( size );
	}

	const char* ConsoleController::getName() const
	{
		return "Controller";
	}

	void ConsoleController::drawInstances( const uint16x2 size ) const
	{
		if( !m_drawInstances )
		{
			return;
		}

		ConsoleRenderer::drawRectangle( size.x - m_instancesWidth, 6u, m_instancesWidth, size.y - 6u, LineType_Single );

		uint16 top = 7u;
		const uint16 left = size.x - m_instancesWidth + 2u;
		ConsoleRenderer::drawText( left, top++, "Instances:" );
		for( const ControllerInstance& instance : m_instances )
		{
			ConsoleRenderer::drawText( left, top++, instance.name.c_str() );
		}
	}

	void ConsoleController::drawLog( const uint16x2 size ) const
	{
		if( !m_drawLog )
		{
			return;
		}

		uint16 top = 7u;
		const uint16 height = size.y - 11u;
		const uint16 bottom = top + height - 1u;
		ConsoleRenderer::drawRectangle( 0u, 6u, size.x - m_instancesWidth, size.y - 9u, LineType_Single );
		ConsoleRenderer::drawFillRectangle( 2u, top, size.x - m_instancesWidth - 4u, height, L' ' );

		if( m_log.size() < height )
		{
			top += uint16( height - m_log.size() - 1u );
		}

		for( const std::string& log : m_log )
		{
			const uint16x2 textSize = ConsoleRenderer::drawTextMultiline( 2u, top, log.c_str() );
			top += textSize.y;

			if( top >= bottom )
			{
				break;
			}
		}
	}

	void ConsoleController::drawCommand( const uint16x2 size ) const
	{
		if( !m_drawCommand )
		{
			return;
		}

		ConsoleRenderer::drawRectangle( 0u, size.y - 3u, size.x - m_instancesWidth, 3u, LineType_Single );

		const size_t maxCommandLength = size.x - 4u - m_instancesWidth;
		const char* pText = m_commandText.c_str();
		if( m_commandText.size() > maxCommandLength )
		{
			pText += ( m_commandText.size() - maxCommandLength );
		}

		ConsoleRenderer::drawText( 2u, size.y - 2u, pText );

		if( m_commandText.size() < maxCommandLength )
		{
			ConsoleRenderer::drawCharacterRepeated( uint16( 2u + m_commandText.size() ), size.y - 2u, ' ', uint16( maxCommandLength - m_commandText.size() ) );
		}

		if( m_commandText.size() <= maxCommandLength )
		{
			ConsoleRenderer::setCursorPosition( uint16( 2u + m_commandText.size() ), size.y - 2u );
		}
	}

	void ConsoleController::updateInstancesWidth()
	{
		uint16 width = ConsoleRenderer::measureTextSize( "Instances:" ).x;
		for( const ControllerInstance& instance : m_instances )
		{
			width = CONCT_MAX( ConsoleRenderer::measureTextSize( instance.name.c_str() ).x, width );
		}
		width += 4u;

		if( width != m_instancesWidth )
		{
			m_instancesWidth	= width;

			m_drawInstances		= true;
			m_drawLog			= true;
			m_drawCommand		= true;
		}
	}

	void ConsoleController::pushLog( const char* pText )
	{
		m_log.push_back( pText );
		m_drawLog = true;
	}

	void ConsoleController::autoComplete( ConsoleDevice& device )
	{

	}

	void ConsoleController::executeCommand( ConsoleDevice& device )
	{
		m_commandHistory.push_back( m_commandText );

		size_t currentIndex = 0u;
		std::string commandText;
		std::vector< std::string > arguments;

		size_t nextIndex = m_commandText.find_first_of( ' ' );
		if( nextIndex != std::string::npos )
		{
			commandText = m_commandText.substr( 0u, nextIndex );
			currentIndex = nextIndex + 1u;
		}
		else
		{
			commandText = m_commandText;
			currentIndex = m_commandText.size();
		}

		size_t startIndex = 0u;
		while( currentIndex < m_commandText.size() )
		{
			while( m_commandText[ currentIndex ] == ' ' )
			{
				currentIndex++;
			}

			char searchChar = ' ';
			if( m_commandText[ currentIndex ] == '"' )
			{
				searchChar = '"';
				currentIndex++;
			}

			startIndex = currentIndex;

			currentIndex = m_commandText.find( searchChar, currentIndex );
			if( currentIndex == std::string::npos )
			{
				arguments.push_back( m_commandText.substr( startIndex ) );
				break;
			}

			arguments.push_back( m_commandText.substr( startIndex, currentIndex - startIndex ) );

			if( searchChar == '"' )
			{
				currentIndex++;
			}
		}

		bool found = false;
		for( size_t i = 0u; i < CONCT_COUNT( s_aCommands ); ++i )
		{
			const CommandInfo& command = s_aCommands[ i ];
			if( command.pCommand != commandText )
			{
				continue;
			}

			(this->*command.pFunction)( device, arguments );
			found = true;
			break;
		}

		if( !found )
		{
			const std::string text = "'" + commandText + "' unknown command";
			pushLog( text.c_str() );
		}

		m_commandText.clear();
		m_drawCommand = true;
	}

	void ConsoleController::executeHelpCommand( ConsoleDevice& device, const std::vector< std::string >& arguments )
	{
		pushLog(   "Commands:" );
		pushLog(   "help			- show this help" );
		pushLog(   "getInstance		- get first instance if given type from device" );
		pushLog(   "more coming soon..." );
	}

	void ConsoleController::executeGetInstanceCommand( ConsoleDevice& device, const std::vector< std::string >& arguments )
	{
		if( arguments.size() != 2u )
		{
			pushLog( "getInstance: not enough arguments. deviceId and typeName are required" );
			return;
		}

		std::stringstream deviceIdStream( arguments[ 0u ] );

		int deviceId;
		deviceIdStream >> deviceId;

		if( !deviceIdStream )
		{
			const std::string text = "getInstance: '" + arguments[ 0u ] + "' is not a valid device id.";
			pushLog( text.c_str() );
			return;
		}

		DeviceAddress address;
		address.address[ 0u ] = (DeviceId)deviceId;
		address.address[ 1u ] = InvalidDeviceId;

		const std::string& typeName = arguments[ 1u ];
		const InterfaceType* pType = m_pTypes->findInterface( typeName, "" );
		if( pType == nullptr )
		{
			const std::string text = "getInstance: type with name '" + typeName + "' not found.";
			pushLog( text.c_str() );
			return;
		}

		Command< RemoteInstance >* pCommand = device.data.pController->getInstance( address, pType->getCrc() );
		if( pCommand == nullptr )
		{
			const std::string text = "getInstance: failed to start command.";
			pushLog( text.c_str() );
			return;
		}

		m_pRunningCommand = pCommand;
	}
}