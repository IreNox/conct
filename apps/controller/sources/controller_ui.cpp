#include "controller_ui.h"

#include "controller_ui_widgets.h"

#include <tiki/tiki_functions.h>
#include <tiki/tiki_string_tools.h>

#include <imapp/imapp.h>

namespace conct
{
	using namespace imui;
	using namespace imui::toolbox;

	void ControllerUI::setup( ControllerConfig& config, ControllerState& state )
	{
		m_config	= &config;
		m_state	= &state;

		m_editTitle.reserve( 256u );
		m_editHostname.reserve( 256u );
		m_editPort.reserve( 32u );
	}

	void ControllerUI::doUI( ImAppContext* imapp, ImUiSurface* rawSurface )
	{
		UiSurface surface( rawSurface );

		const bool showMenuAlways	= (float( surface.getSize().width ) / surface.getSize().height) > 1.5f;
		const bool showMenu			= m_isMenuOpen || showMenuAlways;
		const float menuWidth		= floorf( max( 200.0f, surface.getSize().width / 5.0f ) );

		const UiRect contentRect = surface.getRect().shrinkBorder( UiBorder( 0.0f, showMenu ? menuWidth : 0.0f, 0.0f, 0.0f ) );
		UiToolboxWindow contentWindow( surface, "Content", contentRect, 1u );

		UiWidgetLayoutVertical mainLayout( contentWindow, 5.0f );
		mainLayout.setHStretch( 1.0f );
		mainLayout.setMargin( UiBorder( 10.0f ) );

		//UiAnimation< float > menuAnimation( mainLayout, -menuWidth, 0.0f, 5.0f, !showMenu );
		if( showMenu )
		{
			doMenuUI( imapp, surface, menuWidth, 0.0f ); // menuAnimation.getValue() );
		}

		if( !showMenuAlways )
		{
			const struct ImUiImage image = ImAppImageGetImage( ImAppImageLoadResource( imapp, "icons/menu.png" ) );
			if( contentWindow.buttonIcon( image ) )
			{
				m_isMenuOpen = !m_isMenuOpen;
			}
		}

		{
			UiWidgetLayoutHorizontal titleLayout( contentWindow, 10.0f );
			titleLayout.setHStretch( 1.0f );
			titleLayout.setPadding( UiBorder( 5.0f ) );

			titleLayout.drawSkin( UiToolboxConfig::getSkin( ImUiToolboxSkin_Button ), UiToolboxConfig::getColor( ImUiToolboxColor_Button ) );

			{
				const char* iconName		= getStateIcon( m_menuState );
				const struct ImUiImage img	= ImAppImageGetImage( ImAppImageLoadResource( imapp, iconName ) );
				contentWindow.image( img );
			}

			contentWindow.label( getStateTitle( m_menuState ) );
		}

		switch( m_menuState )
		{
		case ControllerUI::State::Home:
			doHomeUI( contentWindow );
			break;

		case ControllerUI::State::Devices:
			doDevicesUI( imapp, contentWindow );
			break;

		case ControllerUI::State::Connections:
			doConnectionsUI( contentWindow );
			break;

		case ControllerUI::State::ConnectionEdit:
			doConnectionEditUI( contentWindow );
			break;

		case ControllerUI::State::Credits:
			doCreditsUI( contentWindow );
			break;
		}
	}

	void ControllerUI::doMenuUI( ImAppContext* imapp, UiSurface& surface, float menuWidth, float menuOffset )
	{
		UiRect menuRect = surface.getRect();
		menuRect.pos.x += menuOffset;
		menuRect.size.width = menuWidth;

		UiToolboxWindow menuWindow( surface, "Menu", menuRect, 2 );

		//if( !nk_window_has_focus( pNkContext ) )
		//{
		//	m_isMenuOpen = false;
		//}

		UiWidgetLayoutVertical menuLayout( menuWindow, 5.0f );
		menuLayout.setVStretch( 1.0f );

		menuLayout.drawColor( UiColor::CreateGray( 96u ) );

		{
			const ImUiImage image = ImAppImageGetImage( ImAppImageLoadResource( imapp, "bg/menu_header.png" ) );
			//const ImUiImage image = ImAppResPakGetImage( ImAppResourceGetDefaultPak( imapp ), "bg/menu_header.png" );
			if( image.textureData )
			{
				UiSize imageSize;
				imageSize.width		= menuRect.size.width;
				imageSize.height	= (float( image.height ) / image.width) * imageSize.width;

				menuWindow.image( image, imageSize );
			}
		}

		static const State s_menuStates[] =
		{
			State::Home,
			State::Devices,
			State::Connections,
			State::Credits,
		};

		for( size_t i = 0u; i < TIKI_ARRAY_COUNT( s_menuStates ); ++i )
		{
			const State state = s_menuStates[ i ];

			const char* title		= getStateTitle( state );
			const char* iconName	= getStateIcon( state );

			const ImUiImage image = ImAppImageGetImage( ImAppImageLoadResource( imapp, iconName ) );

			UiToolboxButton menuButton( menuWindow );
			menuButton.setHStretch( 1.0f );

			{
				UiWidgetLayoutHorizontal buttonLayout( menuWindow, 5.0f );
				buttonLayout.setVAlign( 0.5f );

				menuWindow.image( image );
				menuWindow.label( title );
			}

			if( menuButton.end() )
			{
				changeState( state );
				m_isMenuOpen = false;
			}
		}
	}

	void ControllerUI::doHomeUI( UiToolboxWindow& window )
	{
		window.label( "TODO" );
	}

	void ControllerUI::doDevicesUI( ImAppContext* imapp, UiToolboxWindow& window )
	{
		UiWidgetLayoutVertical layout( window );

		const ControllerState::DeviceArray& devices = m_state->getDevices();
		for( ControllerState::ConnectedDevice* pDevice : devices )
		{
			doDeviceUI( imapp, window, *pDevice );
		}

		if( devices.hasElements() )
		{
			window.spacer( 1.0f, 20.0f );
		}
	}

	void ControllerUI::doConnectionsUI( UiToolboxWindow& window )
	{
		UiWidgetLayoutVertical layout( window );
		layout.setVStretch( 1.0f );

		const ControllerConfig::ConnectionArray& connections = m_config->getConnections();

		{
			UiToolboxList list( window, 100.0f, connections.getLength() );

			for( const ControllerConfig::Connection& connection : connections )
			{
				list.nextItem();

				UiWidgetLayoutVertical itemLayout( window );

				window.label( connection.hostname );

				window.buttonLabel( "Edit" );
				window.buttonLabel( "Remove" );
			}
		}

		if( window.buttonLabel( "Add Connection" ) )
		{
			m_editTitle = "New Connection"_s;
			m_editHostname.clear();
			m_editPort = "5489"_s;

			changeState( State::ConnectionEdit );
		}
	}

	void ControllerUI::doConnectionEditUI( UiToolboxWindow& window )
	{
		UiWidgetLayoutVertical mainLayout( window );
		mainLayout.setHStretch( 1.0f );

		{
			UiWidgetLayoutGrid layout( window, 2u, 5.0f, 5.0f );
			layout.setHStretch( 1.0f );

			window.label( "Title:" );
			window.textEdit( m_editTitle.beginWrite(), m_editTitle.getCapacity() );
			m_editTitle.endWrite();

			window.label( "Hostname:" );
			window.textEdit( m_editHostname.beginWrite(), m_editHostname.getCapacity() );
			m_editHostname.endWrite();

			window.label( "Port:" );
			window.textEdit( m_editPort.beginWrite(), m_editPort.getCapacity() );
			m_editPort.endWrite();
		}

		UiWidgetLayoutHorizontal buttonLayout( window, 5.0f );

		if( window.buttonLabel( "Ok" ) )
		{
			ControllerConfig::Connection connection;
			connection.title	= m_editTitle;
			connection.hostname	= m_editHostname;
			connection.port		= string_tools::parseUInt16( m_editPort );

			m_config->addConnection( connection );

			changeState( State::Connections );
		}

		if( window.buttonLabel( "Cancel" ) )
		{
			changeState( State::Connections );
		}
	}

	void ControllerUI::doCreditsUI( UiToolboxWindow& window )
	{
		UiWidgetLayoutVertical layout( window );
		layout.setHStretch( 1.0f );

		window.label( "Programming:" );
		window.label( "Tim Boden - https://github.com/IreNox" );

		window.spacer( 1.0f, 20.0f );

		window.label( "Special Thanks:" );
		window.label( "Remix Icon - https://remixicon.com/" );
	}

	void ControllerUI::doDeviceUI( ImAppContext* imapp, UiToolboxWindow& window, ControllerState::ConnectedDevice& device )
	{
		// Title
		{
			//nk_style_push_style_item( pNkContext, &pNkContext->style.window.fixed_background, pNkContext->style.window.header.normal );
			//CONCT_ASSERT( nk_panel_begin( pNkContext, "Bla", NK_PANEL_GROUP ) );

			//nk_layout_row_begin( pNkContext, NK_STATIC, 0.0f, 1 );
			//nk_label( pNkContext, "Bla", NK_TEXT_LEFT );

			//nk_panel_end( pNkContext );
			//nk_style_pop_style_item( pNkContext );

			UiHeader deviceHeader( window, "deviceHeader" );
			if( deviceHeader.isVisible )
			{
				const ImUiImage instancesImage = ImAppImageGetImage( ImAppImageLoadResource( imapp, "icons/instances.png" ) );
				window.image( instancesImage );

				window.label( device.name );
				window.strecher( 1.0f, 0.0f );

				// Favorite Button
				{
					const ImUiImage favImage = ImAppImageGetImage( ImAppImageLoadResource( imapp, "icons/fav-off.png" ) );
					if( window.buttonIcon( favImage ) )
					{

					}
				}
			}
		}

		//for( ControllerState::DeviceInstance& instance : device.instances )
		//{
		//	doInstanceUI( pContext, device, instance );
		//}

		//nk_layout_row_static( pNkContext, 25.0f, 150, 2 );

		//nk_button_label( pNkContext, "Edit" );
		//nk_button_label( pNkContext, "Remove" );

		//nk_group_end( pNkContext );
	}

	void ControllerUI::doInstanceUI( UiToolboxWindow& window, const ControllerState::ConnectedDevice& device, ControllerState::DeviceInstance& instance )
	{
		UiWidgetLayoutVertical layout( window, 5.0f );
		layout.setHStretch( 1.0f );

		const char* typeName = "Unknown Type";
		if( instance.pType != nullptr )
		{
			typeName = instance.pType->getName();
		}

		{
			//nk_layout_row_dynamic( pNkContext, 0.0f, 1 );

			//const ImUiImage instanceImage = ImAppImageGetBlocking( pContext, "icons/instance.png" );
			//ui::doHeaderImageLabel( window, instanceImage, typeName );

			//nk_layout_row_begin( pNkContext, NK_STATIC, 0.0f, 2 );
			//
			//nk_layout_row_push( pNkContext, 24.0f );
			//nk_style_push_vec2( pNkContext, &pNkContext->style.window.spacing, nk_vec2( 0.0f, 9.0f ) );
			//nk_image( pNkContext, instanceImage );
			//nk_style_pop_vec2( pNkContext );
			//
			//nk_label( pNkContext, pTypeName, NK_TEXT_LEFT );
		}

		for( ControllerState::InstanceProperty& prop : instance.properties )
		{
			doPropertyUI( window, device, instance, prop );
		}
	}

	void ControllerUI::doPropertyUI( UiToolboxWindow& window, const ControllerState::ConnectedDevice& device, const ControllerState::DeviceInstance& instance, ControllerState::InstanceProperty& prop )
	{
		UiWidgetLayoutHorizontal layout( window, 5.0f );

		window.label( prop.pProperty->name );

		if( prop.value.getType() == ValueType::Void &&
			prop.pGetCommand != nullptr )
		{
			window.label( "Loading..." );
			return;
		}

		const ValueType valueType = prop.value.getType();
		switch( valueType )
		{
		case ValueType::Void:
			window.label( "No Value" );
			break;

		case ValueType::Boolean:
			{
				bool active = prop.value.getBoolean();
				if( window.checkBox( active, active ? "True" : "False" ) )
				{
					Value newValue;
					newValue.setBoolean( active );

					m_state->changeProperty( device, instance, prop, newValue );
				}
			}
			break;

		case ValueType::Integer:
		case ValueType::Unsigned:
			{
				const sint64 value = valueType == ValueType::Integer ? prop.value.getInteger() : prop.value.getUnsigned();
				window.labelFormat( "%d", value );
			}
			break;

		case ValueType::PercentValue:
			{
				const float oldValue = (float)prop.value.getPercentValue();

				float newValue = oldValue;
				if( window.slider( newValue, 0.0f, 65535.0f ) )
				{
					Value newValue2;
					newValue2.setPercentValue( (PercentValue)newValue );

					m_state->changeProperty( device, instance, prop, newValue2 );
				}
			}
			break;

		case ValueType::DeviceId:
		case ValueType::InstanceId:
		case ValueType::TypeCrc:
		case ValueType::String:
		case ValueType::Struct:
		case ValueType::Array:
			window.label( "Not Implemented" );
			break;

		default:
			break;
		}
	}

	//float ControllerUI::getDeviceHeight( nk_context* pNkContext, const ControllerState::ConnectedDevice& device ) const
	//{
	//	float height = pNkContext->style.window.border;
	//	height += pNkContext->style.font->height;
	//	height += pNkContext->style.window.header.label_padding.y * 2.0f;
	//	height += pNkContext->style.window.header.padding.y * 2.0f;
	//	height += pNkContext->style.window.spacing.y * 2.0f;

	//	for( const ControllerState::DeviceInstance& instance : device.instances )
	//	{
	//		height += getInstanceHeight( pNkContext, instance );
	//	}

	//	return height;
	//}

	//float ControllerUI::getInstanceHeight( nk_context* pNkContext, const ControllerState::DeviceInstance& instance ) const
	//{
	//	float height = pNkContext->style.window.group_border * 2.0f;
	//	height += pNkContext->style.font->height;
	//	height += pNkContext->style.window.header.label_padding.y * 2.0f;
	//	height += pNkContext->style.window.header.padding.y * 2.0f;
	//	height += pNkContext->style.window.spacing.y * 2.0f;

	//	for( const ControllerState::InstanceProperty& prop : instance.properties )
	//	{
	//		height += getPropertyHeight( pNkContext, prop );
	//	}

	//	return height;
	//}

	//float ControllerUI::getPropertyHeight( nk_context* pNkContext, const ControllerState::InstanceProperty& prop ) const
	//{
	//	float height = pNkContext->style.font->height;
	//	height += pNkContext->style.window.padding.y * 2.0f;
	//	height += pNkContext->style.window.spacing.y * 2.0f;

	//	return height;
	//}

	void ControllerUI::changeState( State state )
	{
		m_menuState = state;

		switch( state )
		{
		case State::Home:
			break;

		case State::Devices:
			break;

		case State::Connections:
			break;

		case State::ConnectionEdit:
			break;

		case State::Credits:
			break;
		}
	}

	const char* ControllerUI::getStateTitle( State value )
	{
		switch( value )
		{
		case State::Home:				return "Home";
		case State::Devices:			return "Devices";
		case State::Connections:		return "Connections";
		case State::ConnectionEdit:		return "Edit Connection";
		case State::Credits:			return "Credits";
		}

		return "???";
	}

	const char* ControllerUI::getStateIcon( State value )
	{
		switch( value )
		{
		case State::Home:				return "icons/home.png";
		case State::Devices:			return "icons/devices.png";
		case State::Connections:		return "icons/connections.png";
		case State::ConnectionEdit:		return "icons/edit-connection.png";
		case State::Credits:			return "icons/credits.png";
		}

		return "???";
	}
}
