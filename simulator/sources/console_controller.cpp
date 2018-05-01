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
			setState( State_Value );
		}
	}

	void ConsoleController::deactivate( ConsoleDevice& device )
	{
	}

	void ConsoleController::update( ConsoleDevice& device )
	{
		if( m_state == State_Value )
		{
			updateValue( device );
		}
		else if( m_state == State_Waiting )
		{
			drawLoading();
			finishCommand();
		}
		else
		{
			updateList( device );
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

	void ConsoleController::updateList( ConsoleDevice& device )
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
				previousState();
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
	}

	void ConsoleController::updateValue( ConsoleDevice& device )
	{
		bool hasChanged = false;

		char c;
		if( ConsoleInput::readChar( c ) )
		{
			std::string::iterator it = m_valueText.begin() + m_index;
			m_valueText.insert( it, c );
			m_index++;

			hasChanged = true;
		}

		ConsoleKey key;
		if( ConsoleInput::readKey( key ) )
		{
			switch( key )
			{
			case ConsoleKey_Return:
				nextState( device );
				break;

			case ConsoleKey_Backspace:
				if( m_valueText.empty() )
				{
					previousState();
				}
				else if( m_index > 0u )
				{
					std::string::iterator it = m_valueText.begin() + m_index - 1u;
					if( it == m_valueText.end() )
					{
						it--;
					}
					m_valueText.erase( it );
					m_index--;

					hasChanged = true;
				}
				break;

			case ConsoleKey_Delete:
				if( !m_valueText.empty() && m_index < m_valueText.size() )
				{
					std::string::iterator it = m_valueText.begin() + m_index;
					if( it != m_valueText.end() )
					{
						m_valueText.erase( it );

						hasChanged = true;
					}
				}
				break;

			case ConsoleKey_Left:
				if( m_index > 0u )
				{
					m_index--;
					ConsoleRenderer::setCursorPosition( uint16( 2u + m_index ), 8u );
				}
				break;

			case ConsoleKey_Right:
				if( m_index < m_valueText.size() )
				{
					m_index++;
					ConsoleRenderer::setCursorPosition( uint16( 2u + m_index ), 8u );
				}
				break;

			default:
				break;
			}
		}

		if( hasChanged )
		{
			drawValue();
		}
	}

	void ConsoleController::previousState()
	{
		if( !m_stateHistory.empty() )
		{
			setState( m_stateHistory.back() );
			m_stateHistory.pop_back();
			if( !m_stateHistory.empty() )
			{
				m_stateHistory.pop_back();
			}
		}
	}

	void ConsoleController::nextState( ConsoleDevice& device )
	{
		if( m_list.empty() && m_state != State_Popup && m_state != State_Value )
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
			{
				m_pProperty = &m_pInterface->getProperties()[ m_index ];

				setState( State_Value );
			}
			break;

		case State_Function:
			{
				m_pFunction = &m_pInterface->getFunctions()[ m_index ];

				setState( State_Value );
			}
			break;

		case State_Value:
			{
				Value value;
				if( !setValueFromString( value, m_valueText ) )
				{
					showPopup( "Value can't interpreted." );
					return;
				}

				m_values.push_back( value );

				if( m_action == Action_SetProperty )
				{
					executeAction( device );
				}
				else if( m_action == Action_CallFunction )
				{
					if( m_values.size() < m_pFunction->parameters.size() )
					{
						setState( State_Value );
					}
					else
					{
						executeAction( device );
					}
				}
			}
			break;

		case State_Waiting:
			break;

		case State_Popup:
			if( m_stateHistory.back() != State_Value )
			{
				setState( State_Action );
			}
			else
			{
				drawClear();
				drawValue();
			}
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
		else if( m_state == State_Value )
		{
			m_list.push_back( m_valueText );
		}
		else
		{
			m_list.clear();
		}

		if( m_state != State_Invalid )
		{
			m_stateHistory.push_back( m_state );
		}

		m_state = state;
		m_index = m_lastIndices[ state ];

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

		case State_Value:
			m_valueText.clear();
			drawValue();
			return;

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
		m_popupText = text;
		setState( State_Popup );
	}

	bool ConsoleController::setValueFromString( Value& value, const std::string& text )
	{
		return false;
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
			"Value",
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

	void ConsoleController::drawValue() const
	{
		const uint16x2 size = ConsoleRenderer::getSize();

		ConsoleRenderer::drawRectangle( 0u, 7u, size.x, 3u, LineType_Single );
		ConsoleRenderer::drawText( 2u, 8u, m_valueText.c_str() );
		ConsoleRenderer::drawCharacter( uint16( 2u + m_valueText.size() ), 8u, ' ' );

		ConsoleRenderer::setCursorPosition( uint16( 2u + m_index ), 8u );
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
		const char* pText = m_popupText.c_str();
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
			executeGetPropertyAction( device );
			break;

		case Action_SetProperty:
			executeSetPropertyAction( device );
			break;

		case Action_CallFunction:
			executeCallFunctionAction( device );
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

	void ConsoleController::executeGetPropertyAction( ConsoleDevice& device )
	{
		Command< Value >* pCommand = device.data.pController->getProperty( m_pInstance->instance, m_pProperty->name.c_str() );
		if( pCommand == nullptr )
		{
			showPopup( "Failed to start 'getProperty' command." );
			return;
		}

		m_pRunningCommand = pCommand;
	}

	void ConsoleController::executeSetPropertyAction( ConsoleDevice& device )
	{
		CommandBase* pCommand = device.data.pController->setProperty( m_pInstance->instance, m_pProperty->name.c_str(), m_values.front() );
		if( pCommand == nullptr )
		{
			showPopup( "Failed to start 'setProperty' command." );
			return;
		}

		m_pRunningCommand = pCommand;
	}

	void ConsoleController::executeCallFunctionAction( ConsoleDevice& device )
	{
		ArrayView< Value > arguments;
		arguments.set( m_values.data(), m_values.size() );
		CommandBase* pCommand = device.data.pController->callFunction( m_pInstance->instance, m_pFunction->name.c_str(), arguments );
		if( pCommand == nullptr )
		{
			showPopup( "Failed to start 'setProperty' command." );
			return;
		}

		m_pRunningCommand = pCommand;
	}

	void ConsoleController::finishCommand()
	{
		if( !m_pRunningCommand->isFinish() )
		{
			return;
		}

		if( m_pRunningCommand->hasError() )
		{
			showPopup( "Command failed." );
			m_pRunningCommand = nullptr;
			return;
		}

		switch( m_action )
		{
		case Action_GetInstance:
			{
				Command< RemoteInstance >* pCommand = static_cast< Command< RemoteInstance >* >( m_pRunningCommand );

				ControllerInstance instance;
				instance.instance	= pCommand->getData();
				instance.name		= m_pInterface->getFullName();
				instance.type		= m_pInterface->getCrc();

				m_instances.push_back( instance );
			}
			break;

		case Action_GetProperty:
		case Action_CallFunction:
			{
				Command< Value >* pCommand = static_cast< Command< Value >* >( m_pRunningCommand );

				// show value
			}
			break;

		case Action_SetProperty:
		case Action_Invalid:
		case Action_Count:
			break;
		}

		showPopup( "Command finish." );
		m_pRunningCommand = nullptr;
	}
}