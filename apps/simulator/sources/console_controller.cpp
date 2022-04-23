#include "console_controller.h"

#include "conct_controller.h"
#include "conct_interface_type.h"
#include "conct_router.h"
#include "conct_string_tools.h"
#include "conct_type_collection.h"

#include "console_input.h"
#include "console_render.h"

namespace conct
{
	ConsoleController::ConsoleController( TypeCollection* pTypes )
		: m_pTypes( pTypes )
	{
		m_pDeviceType	= pTypes->findInterface( "Core.Device"_s, ""_s );
		m_state			= State_Invalid;
		m_action		= Action_Invalid;
		m_index			= 0u;

		for( uintreg i = 0u; i < CONCT_COUNT( m_lastIndices ); ++i )
		{
			m_lastIndices[ i ] = 0u;
		}
	}

	void ConsoleController::activate( ConsoleDevice& localDevice )
	{
		if( localDevice.data.pRouter != nullptr )
		{
			const ArrayView< DeviceConnection > devices = localDevice.data.pRouter->getConnectedDevices();
			for( DeviceConnection deviceConnection : devices )
			{
				addDevice( deviceConnection.id, nullptr );
			}
		}

		if( m_state == State_Invalid )
		{
			setState( State_Action );
		}
	}

	void ConsoleController::deactivate( ConsoleDevice& localDevice )
	{
	}

	void ConsoleController::update( ConsoleDevice& localDevice )
	{
		if( m_state == State_Value )
		{
			updateValue( localDevice );
		}
		else if( m_state == State_Waiting )
		{
			drawLoading();
			finishCommand( localDevice );
		}
		else
		{
			updateList( localDevice );
		}
	}

	void ConsoleController::draw( const ConsoleDevice& localDevice ) const
	{
		// ???
	}

	const char* ConsoleController::getName() const
	{
		return "Controller";
	}

	void ConsoleController::updateList( ConsoleDevice& localDevice )
	{
		ConsoleKey key;
		if( ConsoleInput::readKey( key ) )
		{
			switch( key )
			{
			case ConsoleKey_Return:
				nextState( localDevice );
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
				if( m_index < m_list.getLength() - 1u )
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

	void ConsoleController::updateValue( ConsoleDevice& localDevice )
	{
		bool hasChanged = false;

		char c;
		if( ConsoleInput::readChar( c ) )
		{
			m_valueText = m_valueText.insert( c, m_index );
			m_index++;

			hasChanged = true;
		}

		ConsoleKey key;
		if( ConsoleInput::readKey( key ) )
		{
			switch( key )
			{
			case ConsoleKey_Return:
				nextState( localDevice );
				break;

			case ConsoleKey_Backspace:
				if( m_valueText.isEmpty() )
				{
					previousState();
				}
				else if( m_index > 0u )
				{
					uintreg index = m_index - 1u;
					if( index == m_valueText.getLength() )
					{
						index--;
					}
					m_valueText = m_valueText.erase( index );
					m_index--;

					hasChanged = true;
				}
				break;

			case ConsoleKey_Delete:
				if( !m_valueText.isEmpty() && m_index < m_valueText.getLength() )
				{
					m_valueText = m_valueText.erase( m_index );
					hasChanged = true;
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
				if( m_index < m_valueText.getLength() )
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
		if( !m_stateHistory.isEmpty() )
		{
			setState( m_stateHistory.getBack() );
			m_stateHistory.popBack();
			if( !m_stateHistory.isEmpty() )
			{
				m_stateHistory.popBack();
			}
		}
	}

	void ConsoleController::nextState( ConsoleDevice& localDevice )
	{
		if( m_list.isEmpty() && m_state != State_Popup && m_state != State_Value )
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

		//case State_Device:
		//	{
		//		m_pDevice = &m_devices[ m_index ];

		//		switch( m_action )
		//		{
		//		case Action_GetInstance:
		//			setState( State_Type );
		//			break;

		//		case Action_GetProperty:
		//		case Action_SetProperty:
		//		case Action_CallFunction:
		//		case Action_Invalid:
		//		case Action_Count:
		//			setState( State_Action );
		//			break;
		//		}
		//	}
		//	break;

		case State_Instance:
			{
				m_pInstance = nullptr;

				uintreg index = 0u;
				for( const ControllerInstance& instance : m_instances )
				{
					if( index == m_index )
					{
						m_pInstance = &instance;
						break;
					}

					index++;
				}

				switch( m_action )
				{
				case Action_GetProperty:
				case Action_SetProperty:
					setState( State_Property );
					break;

				case Action_CallFunction:
					setState( State_Function );
					break;

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

				executeAction( localDevice );
			}
			break;

		case State_Property:
			{
				m_pProperty = &m_pInstance->pType->getProperties()[ m_index ];

				if( m_action == Action_SetProperty )
				{
					if( m_pProperty->pType->getKind() == TypeKind_Value )
					{
						setValueState( m_pProperty->pType->getValueType() );
					}
					else
					{
						setState( State_Instance );
					}
				}
				else
				{
					executeAction( localDevice );
				}
			}
			break;

		case State_Function:
			{
				m_pFunction = &m_pInstance->pType->getFunctions()[ m_index ];

				if( !m_pFunction->parameters.isEmpty() )
				{
					setValueState( m_pFunction->parameters[ 0u ].pType->getValueType() );
				}
				else
				{
					executeAction( localDevice );
				}
			}
			break;

		case State_Value:
			{
				ValueHigh value;
				if( !setValueFromString( value, m_valueText ) )
				{
					setPopupState( "Value can't interpreted."_s );
					return;
				}

				m_values.pushBack( value );

				if( m_action == Action_SetProperty )
				{
					executeAction( localDevice );
				}
				else if( m_action == Action_CallFunction )
				{
					if( m_values.getLength() < m_pFunction->parameters.getLength() )
					{
						const ValueType nextType = m_pFunction->parameters[ m_values.getLength() ].pType->getValueType();
						setValueState( nextType );
					}
					else
					{
						executeAction( localDevice );
					}
				}
			}
			break;

		case State_Waiting:
			break;

		case State_Popup:
			if( m_stateHistory.getBack() != State_Value )
			{
				setState( State_Action );
			}
			else
			{
				drawClear();
				drawValue();

				m_state = State_Value;
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
		else if( m_state == State_Value && state == State_Value )
		{
			m_list.pushBack( m_valueText );
		}
		else
		{
			m_list.clear();
		}

		if( m_state != State_Invalid )
		{
			m_stateHistory.pushBack( m_state );
		}

		m_state = state;
		if( m_state != State_Popup )
		{
			m_index = m_lastIndices[ state ];
		}

		switch( state )
		{
		case State_Action:
			buildActions();
			break;

		//case State_Device:
		//	buildDevices();
		//	break;

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

		if( m_index >= m_list.getLength() )
		{
			m_index = 0u;
			m_lastIndices[ state ] = 0u;
		}

		drawList();
	}

	void ConsoleController::setValueState( ValueType targetType )
	{
		m_valueType = targetType;
		setState( State_Value );
	}

	void ConsoleController::setPopupState( const DynamicString& text )
	{
		m_popupText = text;
		setState( State_Popup );
	}

	bool ConsoleController::setValueFromString( ValueHigh& value, const DynamicString& text ) const
	{
		switch( m_valueType )
		{
		case ValueType_Void:
			return false;

		case ValueType_Boolean:
			{
				const DynamicString valueText = text.trim().toLower();
				if( valueText == "false" || valueText == "0" )
				{
					value.setBoolean( false );
					return true;
				}
				else if( valueText == "true" || valueText == "1" )
				{
					value.setBoolean( false );
					return true;
				}
			}
			break;

		case ValueType_Integer:
			{
				const DynamicString valueText = text.trim();

				sint32 intValue;
				if( string_tools::tryParseSInt32( intValue, valueText.toConstCharPointer() ) )
				{
					value.setInteger( intValue );
					return true;
				}
			}
			break;

		case ValueType_Unsigned:
			{
				const DynamicString valueText = text.trim();

				uint32 intValue;
				if( string_tools::tryParseUInt32( intValue, valueText.toConstCharPointer() ) )
				{
					value.setInteger( intValue );
					return true;
				}
			}
			break;

		case ValueType_String:
			//value.setString();
			break;

		case ValueType_PercentValue:
			{
				const DynamicString valueText = text.trim();

				if( valueText == "1" )
				{
					value.setPercentValue( 65335u );
					return true;
				}

				float floatValue;
				if( string_tools::tryParseFloat( floatValue, valueText.toConstCharPointer() ) && floatValue >= 0.0f )
				{
					if( floatValue <= 1.0f )
					{
						value.setPercentValue( PercentValue( floatValue * 65535.0f ) );
						return true;
					}
					else if( floatValue <= 65535.0f )
					{
						value.setPercentValue( PercentValue( floatValue ) );
						return true;
					}
				}
			}
			break;

		//case ValueType_Instance:
		//	{
		//		InstanceId intValue;
		//		if( string_tools::tryParseUInt16( intValue, text.toConstCharPointer() ) )
		//		{
		//			value.setInstance( intValue );
		//			return true;
		//		}
		//	}
		//	break;

		case ValueType_TypeCrc:
			{
				TypeCrc intValue;
				if( string_tools::tryParseUInt16( intValue, text.toConstCharPointer() ) )
				{
					value.setTypeCrc( intValue );
					return true;
				}
			}
			break;
		}

		return false;
	}

	DynamicString ConsoleController::getStringFromValue( const ValueHigh& value ) const
	{
		switch( value.getType() )
		{
		case ValueType_Void:
			return "void"_s;

		case ValueType_Boolean:
			return DynamicString( value.getBoolean() ? "true" : "false" );
			break;

		case ValueType_Integer:
			return string_tools::toString( value.getInteger() );
			break;

		case ValueType_Unsigned:
			return string_tools::toString( value.getUnsigned() );
			break;

		case ValueType_String:
			return DynamicString( value.getString() );
			break;

		case ValueType_PercentValue:
			return string_tools::toString( float( value.getPercentValue() ) * ( 100.0f / 65535.0f ) ) + " %";
			break;

		case ValueType_InstanceId:
			return string_tools::toString( value.getInstanceId() );
			break;

		case ValueType_TypeCrc:
			return string_tools::toString( value.getTypeCrc() );
			break;
		}

		return DynamicString();
	}

	DynamicString ConsoleController::getInstanceName( const ControllerInstance& instance ) const
	{
		if( instance.pType == nullptr )
		{
			return "Unknown @ "_s + instance.pDevice->name;
		}

		return instance.pType->getFullName() + " @ " + instance.pDevice->name;
	}

	DynamicString ConsoleController::addDevice( DeviceId deviceId, const DeviceAddress* pBaseAddress )
	{
		DeviceAddress address;
		if( pBaseAddress != 0u )
		{
			address = *pBaseAddress;
		}
		else
		{
			address.address[ 0u ] = InvalidDeviceId;
		}

		for( uintreg i = 0u; i < DeviceAddress::Size - 1u; ++i )
		{
			if( address.address[ i ] != InvalidDeviceId )
			{
				continue;
			}

			address.address[ i ] = deviceId;
			address.address[ i + 1u ] = InvalidDeviceId;
			break;
		}

		for( const ControllerDevice& currentDevice : m_devices )
		{
			if( runtime::isDeviceAddressEquals( address, currentDevice.address ) )
			{
				return "\nDevice: "_s + currentDevice.name + " already there.";
			}
		}

		ControllerDevice newDevice;
		newDevice.address = address;

		for( uintreg i = 0u; i < DeviceAddress::Size; ++i )
		{
			if( address.address[ i ] == InvalidDeviceId )
			{
				break;
			}

			if( !newDevice.name.isEmpty() )
			{
				newDevice.name += "-";
			}

			newDevice.name += string_tools::toString( address.address[ i ] );
		}

		m_devices.push_front( newDevice );

		ControllerInstance instance;
		instance.pDevice				= &m_devices.front();
		instance.instanceId				= 0u;
		instance.pType					= m_pDeviceType;
		m_instances.push_front( instance );

		return "\nDevice added: "_s + newDevice.name + ".";
	}

	DynamicString ConsoleController::addInstance( Instance sourceInstance )
	{
		for( const ControllerInstance& instance : m_instances )
		{
			if( instance.instanceId == sourceInstance.id &&
				instance.pDevice == m_pInstance->pDevice &&
				instance.pType->getCrc() == sourceInstance.type )
			{
				return "\nInstance: "_s + getInstanceName( instance ) + " already there.";
			}
		}

		const InterfaceType* pType = m_pTypes->findInterfaceByCrc( sourceInstance.type );
		if( pType == nullptr )
		{
			return "\nCould not find type with crc "_s + string_tools::toString( sourceInstance.type ) + "."_s;
		}

		ControllerInstance instance;
		instance.pDevice			= m_pInstance->pDevice;
		instance.instanceId			= sourceInstance.id;
		instance.pType				= pType;
		m_instances.push_front( instance );

		return "\nInstance added: "_s + getInstanceName( instance ) + ".";
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
			//"Device",
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
		for( const DynamicString& text : m_list )
		{
			const uint16 right = uint16( x + text.getLength() + 4u );
			if( right >= size.x )
			{
				x = 0u;
				y += 3u;
			}

			x += ConsoleRenderer::drawButton( x, y, text.toConstCharPointer(), i == m_index ? LineType_Double : LineType_Single );

			i++;
		}
	}

	void ConsoleController::drawValue() const
	{
		const uint16x2 size = ConsoleRenderer::getSize();

		DynamicString titleText = "Please enter "_s;
		titleText += getValueTypeName( m_valueType );
		titleText += ":";

		ConsoleRenderer::drawText( 0u, 6u, titleText.toConstCharPointer() );
		ConsoleRenderer::drawRectangle( 0u, 7u, size.x, 3u, LineType_Single );
		ConsoleRenderer::drawText( 2u, 8u, m_valueText.toConstCharPointer() );

		const uint16 textEndX = uint16( 2u + m_valueText.getLength() );
		ConsoleRenderer::drawCharacterRepeated( textEndX, 8u, ' ', size.x - textEndX - 2u );

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
		const char* pText = m_popupText.toConstCharPointer();
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
			"Get Property"_s,
			"Set Property"_s,
			"Call Function"_s,
		};
	}

	//void ConsoleController::buildDevices()
	//{
	//	for( const ControllerDevice& localDevice : m_devices )
	//	{
	//		m_list.pushBack( localDevice.name );
	//	}
	//}

	void ConsoleController::buildInstances()
	{
		for( const ControllerInstance& instance : m_instances )
		{
			m_list.pushBack( getInstanceName( instance ) );
		}
	}

	void ConsoleController::buildTypes()
	{
		for( const InterfaceType* pInterface : m_pTypes->getInterfaces() )
		{
			m_list.pushBack( pInterface->getFullName() );
		}
	}

	void ConsoleController::buildProperties()
	{
		for( const InterfaceProperty& property : m_pInstance->pType->getProperties() )
		{
			m_list.pushBack( property.name );
		}
	}

	void ConsoleController::buildFunctions()
	{
		for( const InterfaceFunction& function : m_pInstance->pType->getFunctions() )
		{
			m_list.pushBack( function.name );
		}
	}

	void ConsoleController::executeAction( ConsoleDevice& localDevice )
	{
		RemoteInstance remoteInstance;
		remoteInstance.id		= m_pInstance->instanceId;
		remoteInstance.address	= m_pInstance->pDevice->address;

		switch( m_action )
		{
		case Action_GetProperty:
			executeGetPropertyAction( localDevice, remoteInstance );
			break;

		case Action_SetProperty:
			executeSetPropertyAction( localDevice, remoteInstance );
			break;

		case Action_CallFunction:
			executeCallFunctionAction( localDevice, remoteInstance );
			break;

		case Action_Invalid:
		case Action_Count:
			break;
		}

		if( m_pRunningCommand != nullptr )
		{
			setState( State_Waiting );
		}
	}

	void ConsoleController::executeGetPropertyAction( ConsoleDevice& localDevice, const RemoteInstance& remoteInstance )
	{
		ValueCommand* pCommand = localDevice.data.pController->getProperty( remoteInstance, m_pProperty->name.toConstCharPointer() );
		if( pCommand == nullptr )
		{
			setPopupState( "Failed to start 'getProperty' command."_s );
			return;
		}

		m_pRunningCommand = pCommand;
	}

	void ConsoleController::executeSetPropertyAction( ConsoleDevice& localDevice, const RemoteInstance& remoteInstance )
	{
		Command* pCommand = localDevice.data.pController->setProperty( remoteInstance, m_pProperty->name.toConstCharPointer(), m_values.getFront() );
		if( pCommand == nullptr )
		{
			setPopupState( "Failed to start 'setProperty' command."_s );
			return;
		}

		m_pRunningCommand = pCommand;
	}

	void ConsoleController::executeCallFunctionAction( ConsoleDevice& localDevice, const RemoteInstance& remoteInstance )
	{
		ArrayView< ValueHigh > arguments;
		arguments.set( m_values.getData(), m_values.getLength() );
		Command* pCommand = localDevice.data.pController->callFunction( remoteInstance, m_pFunction->name.toConstCharPointer(), arguments );
		if( pCommand == nullptr )
		{
			setPopupState( "Failed to start 'setProperty' command."_s );
			return;
		}

		m_pRunningCommand = pCommand;
	}

	void ConsoleController::finishCommand( ConsoleDevice& localDevice )
	{
		if( !m_pRunningCommand->isFinish() )
		{
			return;
		}

		if( m_pRunningCommand->isFailure() )
		{
			setPopupState( "Command failed."_s );
			localDevice.data.pController->releaseCommand( m_pRunningCommand );
			m_pRunningCommand = nullptr;
			return;
		}

		switch( m_action )
		{
		//case Action_GetInstance:
		//	{
		//		Command< RemoteInstance >* pCommand = static_cast< Command< RemoteInstance >* >( m_pRunningCommand );

		//		ControllerInstance instance;
		//		instance.instance	= pCommand->getData();
		//		instance.name		= DynamicString( m_pInterface->getFullName().c_str() ) + " @ " + m_pDevice->name;
		//		instance.pType		= m_pInterface;

		//		m_instances.pushBack( instance );
		//	}
		//	break;

		case Action_GetProperty:
		case Action_CallFunction:
			{
				ValueCommand* pCommand = static_cast< ValueCommand* >( m_pRunningCommand );
				const ValueHigh& value = pCommand->getValue();

				DynamicString text;
				if( m_action == Action_GetProperty )
				{
					text = "Property '"_s + m_pProperty->name + "' has the following value:\n"_s;
				}
				else
				{
					text = "Function '"_s + m_pFunction->name + "' returned the following value:\n"_s;
				}
				text += getStringFromValue( value );

				if( m_action == Action_GetProperty && value.getType() == ValueType_String && m_pInstance->pType == m_pDeviceType && m_pProperty->name == "Name" )
				{
					m_pInstance->pDevice->name = DynamicString( value.getString() );
				}
				else if( value.getType() == ValueType_Struct && value.getStructType() == ValueTypeTraits< Instance >::getTypeCrc() )
				{
					const Instance sourceInstance = value.getStruct< Instance >();
					text += addInstance( sourceInstance );
				}
				else if( value.getType() == ValueType_Array )
				{
					if( value.getArrayType() == ValueTypeTraits< Instance >::getTypeCrc() )
					{
						const ArrayView< Instance > instances = value.getArray< Instance >();
						for( uintreg i = 0u; i < instances.getLength(); ++i )
						{
							text += addInstance( instances[ i ] );
						}
					}
					else if( value.getArrayType() == ValueTypeTraits< DeviceConnection >::getTypeCrc() )
					{
						const ArrayView< DeviceConnection > devices = value.getArray< DeviceConnection >();
						for( uintreg i = 0u; i < devices.getLength(); ++i )
						{
							text += addDevice( devices[ i ].id, &m_pInstance->pDevice->address );
						}
					}
				}

				setPopupState( text );
			}
			break;

		case Action_SetProperty:
		case Action_Invalid:
		case Action_Count:
			break;
		}

		if( m_state != State_Popup )
		{
			setPopupState( "Command finish."_s );
		}

		localDevice.data.pController->releaseCommand( m_pRunningCommand );
		m_pRunningCommand = nullptr;
	}
}