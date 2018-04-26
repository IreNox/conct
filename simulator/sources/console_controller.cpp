#include "console_controller.h"

#include "type_collection.h"
#include "interface_type.h"

#include "conct_controller.h"

#include "console_input.h"
#include "console_render.h"

#include <sstream>

namespace conct
{
	ConsoleController::ConsoleController( TypeCollection* pTypes )
		: m_pTypes( pTypes )
	{
		m_state		= State_Invalid;
		m_action	= Action_Invalid;
		m_index		= 0u;

		for( uintreg i = 0u; i < CONCT_COUNT( m_lastIndices ); ++i )
		{
			m_lastIndices[ i ] = 0u;
		}

		ControllerDevice device;
		device.name						= "Test";
		device.address.address[ 0u ]	= 1u;
		device.address.address[ 1u ]	= 0u;
		m_devices.push_back( device );
	}

	void ConsoleController::activate( ConsoleDevice& device )
	{
		if( m_state == State_Invalid )
		{
			setState( State_Action );
		}
	}

	void ConsoleController::deactivate( ConsoleDevice& device )
	{
	}

	void ConsoleController::update( ConsoleDevice& device )
	{
		ConsoleKey key;
		if( ConsoleInput::readKey( key ) )
		{
			switch( key )
			{
			case ConsoleKey_Return:
				nextState( device );
				break;

			case ConsoleKey_Backspace:
				if( !m_stateHistory.empty() )
				{
					setState( m_stateHistory.back() );
					m_stateHistory.pop_back();
					if( !m_stateHistory.empty() )
					{
						m_stateHistory.pop_back();
					}
				}
				break;

			case ConsoleKey_Up:
				break;

			case ConsoleKey_Down:
				break;

			case ConsoleKey_Left:
				if( m_index > 0u )
				{
					m_index--;
					drawList();
				}
				break;

			case ConsoleKey_Right:
				if( m_index < m_list.size() )
				{
					m_index++;
					drawList();
				}
				break;

			default:
				break;
			}
		}

		if( m_state == State_Waiting )
		{
			drawLoading();

			if( m_pRunningCommand != nullptr && m_pRunningCommand->isFinish() )
			{
				showPopup( "Command finish." );
				m_pRunningCommand = nullptr;
			}
		}
	}

	void ConsoleController::draw( const ConsoleDevice& device ) const
	{
		// ???
	}

	const char* ConsoleController::getName() const
	{
		return "Controller";
	}

	void ConsoleController::nextState( ConsoleDevice& device )
	{
		if( m_list.empty() )
		{
			return;
		}

		switch( m_state )
		{
		case State_Action:
			{
				m_action = ( Action )m_index;

				switch( m_action )
				{
				case Action_GetInstance:
					setState( State_Device );
					break;

				case Action_GetProperty:
				case Action_SetProperty:
				case Action_CallFunction:
					setState( State_Instance );
					break;

				case Action_Invalid:
				case Action_Count:
					setState( State_Action );
					break;
				}
			}
			break;

		case State_Device:
			{
				m_pDevice = &m_devices[ m_index ];

				switch( m_action )
				{
				case Action_GetInstance:
					setState( State_Type );
					break;

				case Action_GetProperty:
				case Action_SetProperty:
				case Action_CallFunction:
				case Action_Invalid:
				case Action_Count:
					setState( State_Action );
					break;
				}
			}
			break;

		case State_Instance:
			{
				m_pInstance = &m_instances[ m_index ];

				switch( m_action )
				{
				case Action_GetProperty:
				case Action_SetProperty:
					setState( State_Property );
					break;

				case Action_CallFunction:
					setState( State_Function );
					break;

				case Action_GetInstance:
				case Action_Invalid:
				case Action_Count:
					setState( State_Action );
					break;
				}
			}
			break;

		case State_Type:
			{
				m_pInterface = m_pTypes->getInterfaces()[ m_index ];

				executeAction( device );
			}
			break;

		case State_Property:
			break;

		case State_Function:
			break;

		case State_Waiting:
			break;

		case State_Popup:
			setState( State_Action );
			break;

		case State_Invalid:
		case State_Count:
			break;
		}
	}

	void ConsoleController::setState( State state )
	{
		if( state == State_Action )
		{
			m_stateHistory.clear();
		}

		if( m_state != State_Invalid )
		{
			m_stateHistory.push_back( m_state );
		}

		m_state = state;
		m_index = m_lastIndices[ state ];

		if( m_state != State_Popup )
		{
			m_list.clear();
		}

		switch( state )
		{
		case State_Action:
			buildActions();
			break;

		case State_Device:
			buildDevices();
			break;

		case State_Instance:
			buildInstances();
			break;

		case State_Type:
			buildTypes();
			break;

		case State_Property:
			buildProperties();
			break;

		case State_Function:
			buildFunctions();
			break;

		case State_Waiting:
			drawClear();
			drawLoading();
			return;

		case State_Popup:
			drawPopup();
			return;

		case State_Count:
			break;
		}

		if( m_index >= m_list.size() )
		{
			m_index = 0u;
			m_lastIndices[ state ] = 0u;
		}

		drawList();
	}

	void ConsoleController::showPopup( const std::string& text )
	{
		m_list.clear();
		m_list.push_back( text );

		setState( State_Popup );
	}

	void ConsoleController::drawClear() const
	{
		const uint16x2 size = ConsoleRenderer::getSize();

		ConsoleRenderer::drawFillRectangle( 0u, 6u, size.x, size.y - 6u, ' ' );
	}

	void ConsoleController::drawList() const
	{
		static const char* s_aStateNames[] =
		{
			"Action",
			"Device",
			"Instance",
			"Type",
			"Property",
			"Function",
			"Waiting",
			"Popup"
		};
		static_assert( CONCT_COUNT( s_aStateNames ) == State_Count, "" );

		drawClear();
		ConsoleRenderer::drawText( 0u, 6u, s_aStateNames[ m_state ] );

		uint16 x = 0u;
		uint16 y = 7u;
		uintreg i = 0u;
		const uint16x2 size = ConsoleRenderer::getSize();
		for( const std::string& text : m_list )
		{
			const uint16 right = uint16( x + text.size() + 4u );
			if( right >= size.x )
			{
				x = 0u;
				y += 3u;
			}

			x += ConsoleRenderer::drawButton( x, y, text.c_str(), i == m_index ? LineType_Double : LineType_Single );

			i++;
		}
	}

	void ConsoleController::drawLoading() const
	{
		static const char* s_aFrames[] =
		{
			"[    ]",
			"[=   ]",
			"[==  ]",
			"[=== ]",
			"[ ===]",
			"[  ==]",
			"[   =]",
			"[    ]",
			"[   =]",
			"[  ==]",
			"[ ===]",
			"[====]",
			"[=== ]",
			"[==  ]",
			"[=   ]"
		};

		const uint16x2 size = ConsoleRenderer::getSize();

		const uintreg index = uintreg( m_timer.getElapsedSeconds() * ( 1000.0 / 80.0 ) ) % CONCT_COUNT( s_aFrames );
		ConsoleRenderer::drawText( ( size.x / 2u ) - 3u, 7u, s_aFrames[ index ] );
	}

	void ConsoleController::drawPopup() const
	{
		const char* pText = m_list.front().c_str();
		const uint16x2 textSize = ConsoleRenderer::measureTextSize( pText );

		const uint16x2 size = ConsoleRenderer::getSize();

		const uint16 width = CONCT_MAX( textSize.x + 4u, 6u );
		const uint16 height = textSize.y + 7u;
		const uint16 x = ( size.x / 2u ) - ( width / 2u );

		ConsoleRenderer::drawFillRectangle( x, 6u, width, height, ' ' );
		ConsoleRenderer::drawRectangle( x, 6u, width, height, LineType_Single );
		ConsoleRenderer::drawTextMultiline( x + 2u, 6u, pText );
		ConsoleRenderer::drawButton( x + 2u, 9u + textSize.y, "Ok", LineType_Double );
	}

	void ConsoleController::buildActions()
	{
		m_list = {
			"Get Instance",
			"Get Property",
			"Set Property",
			"Call Function",
		};
	}

	void ConsoleController::buildDevices()
	{
		for( const ControllerDevice& device : m_devices )
		{
			m_list.push_back( device.name );
		}
	}

	void ConsoleController::buildInstances()
	{
		for( const ControllerInstance& instance : m_instances )
		{
			m_list.push_back( instance.name );
		}
	}

	void ConsoleController::buildTypes()
	{
		for( const InterfaceType* pInterface : m_pTypes->getInterfaces() )
		{
			m_list.push_back( pInterface->getFullName() );
		}
	}

	void ConsoleController::buildProperties()
	{
		for( const InterfaceProperty& property : m_pInterface->getProperties() )
		{
			m_list.push_back( property.name );
		}
	}

	void ConsoleController::buildFunctions()
	{
		for( const InterfaceFunction& function : m_pInterface->getFunctions() )
		{
			m_list.push_back( function.name );
		}
	}

	void ConsoleController::executeAction( ConsoleDevice& device )
	{
		switch( m_action )
		{
		case Action_GetInstance:
			executeGetInstanceAction( device );
			break;

		case Action_GetProperty:
			break;

		case Action_SetProperty:
			break;

		case Action_CallFunction:
			break;

		case Action_Invalid:
		case Action_Count:
			break;
		}

		setState( State_Waiting );
	}

	void ConsoleController::executeGetInstanceAction( ConsoleDevice& device )
	{
		Command< RemoteInstance >* pCommand = device.data.pController->getInstance( m_pDevice->address, m_pInterface->getCrc() );
		if( pCommand == nullptr )
		{
			showPopup( "Failed to start 'getInstance' command." );
			return;
		}

		m_pRunningCommand = pCommand;
	}
}