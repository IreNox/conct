#include "controller_ui.h"

#include "conct_string_tools.h"

#include "controller_ui_widgets.h"

#include <imapp/imapp.h>

namespace conct
{
	void ControllerUI::setup( ControllerConfig& config, ControllerState& state )
	{
		m_pConfig	= &config;
		m_pState	= &state;

		m_editTitle.reserve( 256u );
		m_editHostname.reserve( 256u );
		m_editPort.reserve( 32u );
	}

	void ControllerUI::doUI( ImAppContext* pContext )
	{
		nk_context* pNkContext = pContext->nkContext;

		const bool showMenuAlways = float( pContext->width ) / pContext->height > 1.5f;
		if( m_isMenuOpen || showMenuAlways )
		{
			doMenuUI( pContext );
		}

		const int x = pContext->x + (showMenuAlways ? pContext->width / 5 : 0);
		const int w = pContext->width - (showMenuAlways ? pContext->width / 5 : 0);
		if( !nk_begin( pNkContext, "Default", nk_recti( x, pContext->y, w, pContext->height ), NK_WINDOW_NO_SCROLLBAR ) )
		{
			return;
		}

		const float height = 24.0f + pNkContext->style.window.padding.y;
		nk_layout_row_begin( pNkContext, NK_STATIC, height, showMenuAlways? 2 : 3 );

		if( !showMenuAlways )
		{
			nk_layout_row_push( pNkContext, 24.0f + pNkContext->style.button.padding.x * 2.0f );

			const struct nk_image image	= ImAppImageGetBlocking( pContext, "icons/menu.png" );
			if( nk_button_image( pNkContext, image ) )
			{
				m_isMenuOpen = !m_isMenuOpen;
			}
		}

		nk_layout_row_push( pNkContext, 28.0f );
		{
			const char* pIcon			= getStateIcon( m_state );
			const struct nk_image image	= ImAppImageGetBlocking( pContext, pIcon );
			const struct nk_color color	= { 0xff, 0xff, 0xff, 0xff };
			nk_image_color( pNkContext, image, color );
		}

		nk_layout_row_push( pNkContext, 500.0f );
		{
			const char* pTitle = getStateTitle( m_state );
			nk_label( pNkContext, pTitle, NK_TEXT_LEFT );
		}

		nk_layout_row_end( pNkContext );

		switch( m_state )
		{
		case ControllerUI::State::Home:
			doHomeUI( pContext );
			break;

		case ControllerUI::State::Devices:
			doDevicesUI( pContext );
			break;

		case ControllerUI::State::Connections:
			doConnectionsUI( pContext );
			break;

		case ControllerUI::State::ConnectionEdit:
			doConnectionEditUI( pContext );
			break;

		case ControllerUI::State::Credits:
			doCreditsUI( pContext );
			break;
		}

		nk_end( pNkContext );
	}

	void ControllerUI::doMenuUI( ImAppContext* pContext )
	{
		nk_context* pNkContext = pContext->nkContext;

		const int menuWidth = max( 200, pContext->width / 5 );
		if( !nk_begin( pNkContext, "Menu", nk_recti( pContext->x, pContext->y, menuWidth, pContext->height ), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR ) )
		{
			m_isMenuOpen = false;
			return;
		}

		if( !nk_window_has_focus( pNkContext ) )
		{
			m_isMenuOpen = false;
		}

		nk_layout_row_dynamic( pNkContext, menuWidth * (723.0f / 1080.0f), 1 );
		{
			const struct nk_image image	= ImAppImageGetBlocking( pContext, "bg/menu_header.png" );
			const struct nk_color color	= { 0xff, 0xff, 0xff, 0xff };
			nk_image_color( pNkContext, image, color );
		}

		nk_layout_row_dynamic( pNkContext, 0.0f, 1 );

		static const State s_menuStates[] =
		{
			State::Home,
			State::Devices,
			State::Connections,
			State::Credits,
		};

		for( size_t i = 0u; i < CONCT_COUNT( s_menuStates ); ++i )
		{
			const State state = s_menuStates[ i ];

			const char* pTitle	= getStateTitle( state );
			const char* pIcon	= getStateIcon( state );

			const struct nk_image image = ImAppImageGetBlocking( pContext, pIcon );
			if( nk_button_image_label( pNkContext, image, pTitle, NK_TEXT_RIGHT ) )
			{
				changeState( state );
				m_isMenuOpen = false;
			}
		}

		nk_end( pNkContext );
	}

	void ControllerUI::doHomeUI( ImAppContext* pContext )
	{
		nk_context* pNkContext = pContext->nkContext;

		nk_layout_row_dynamic( pNkContext, 0.0f, 1 );

		nk_label( pNkContext, "TODO", NK_TEXT_LEFT );
	}

	void ControllerUI::doDevicesUI( ImAppContext* pContext )
	{
		nk_context* pNkContext = pContext->nkContext;

		nk_layout_row_dynamic( pNkContext, pContext->height - 24.0f, 1 );

		nk_style_push_vec2( pNkContext, &pNkContext->style.window.group_padding, nk_vec2i( 0, 0 ) );
		CONCT_ASSERT( nk_group_begin( pNkContext, "devices", 0 ) );

		const ControllerState::DeviceVector& devices = m_pState->getDevices();
		for( ControllerState::ConnectedDevice* pDevice : devices )
		{
			doDeviceUI( pContext, *pDevice );
		}

		if( devices.hasElements() )
		{
			nk_spacing( pNkContext, 1u );
		}

		nk_group_end( pNkContext );
		nk_style_pop_vec2( pNkContext );
	}

	void ControllerUI::doConnectionsUI( ImAppContext* pContext )
	{
		nk_context* pNkContext = pContext->nkContext;

		const ControllerConfig::ConnectionVector& connections = m_pConfig->getConnections();

		nk_layout_row_dynamic( pNkContext, pContext->height - 100.0f, 1 );

		nk_list_view listView;
		if( nk_list_view_begin( pNkContext, &listView, "connections", 0, 100, (int)connections.getLength() ) )
		{
			nk_layout_row_dynamic( pNkContext, 100.0f, 1 );
			for( const ControllerConfig::Connection& connection : connections )
			{
				nk_group_begin( pNkContext, connection.title.toConstCharPointer(), NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR );

				nk_layout_row_dynamic( pNkContext, 25.0f, 1 );

				nk_label( pNkContext, connection.hostname.toConstCharPointer(), NK_TEXT_LEFT );

				nk_layout_row_static( pNkContext, 25.0f, 150, 2 );

				nk_button_label( pNkContext, "Edit" );
				nk_button_label( pNkContext, "Remove" );

				nk_group_end( pNkContext );
			}
			nk_list_view_end( &listView );
		}

		nk_layout_row_static( pNkContext, 0.0f, 150, 1 );

		if( nk_button_label( pNkContext, "Add Connection" ) )
		{
			m_editTitle = "New Connection"_s;
			m_editHostname.clear();
			m_editPort = "5489"_s;

			changeState( State::ConnectionEdit );
		}
	}

	void ControllerUI::doConnectionEditUI( ImAppContext* pContext )
	{
		nk_context* pNkContext = pContext->nkContext;

		const float rations[] = { 0.1f, 0.9f };
		nk_layout_row( pNkContext, NK_DYNAMIC, 0.0f, 2, rations );

		nk_label( pNkContext, "Title:", NK_TEXT_LEFT );
		nk_edit_string_zero_terminated( pNkContext, NK_EDIT_FIELD, m_editTitle.beginWrite(), (int)m_editTitle.getCapacity(), nullptr );
		m_editTitle.endWrite();

		nk_label( pNkContext, "Hostname:", NK_TEXT_LEFT );
		nk_edit_string_zero_terminated( pNkContext, NK_EDIT_FIELD, m_editHostname.beginWrite(), (int)m_editHostname.getCapacity(), nullptr );
		m_editHostname.endWrite();

		nk_label( pNkContext, "Port:", NK_TEXT_LEFT );
		nk_edit_string_zero_terminated( pNkContext, NK_EDIT_FIELD, m_editPort.beginWrite(), (int)m_editPort.getCapacity(), nullptr );
		m_editPort.endWrite();

		nk_layout_row_static( pNkContext, 0.0f, 150, 2 );

		if( nk_button_label( pNkContext, "Ok" ) )
		{
			ControllerConfig::Connection connection;
			connection.title	= m_editTitle;
			connection.hostname	= m_editHostname;
			connection.port		= string_tools::parseUInt16( m_editPort.toConstCharPointer() );

			m_pConfig->addConnection( connection );

			changeState( State::Connections );
		}

		if( nk_button_label( pNkContext, "Cancel" ) )
		{
			changeState( State::Connections );
		}
	}

	void ControllerUI::doCreditsUI( ImAppContext* pContext )
	{
		nk_context* pNkContext = pContext->nkContext;

		nk_layout_row_dynamic( pNkContext, 0.0f, 1 );

		nk_label( pNkContext, "Programming:", NK_TEXT_LEFT );
		nk_label( pNkContext, "Tim Boden - https://github.com/IreNox", NK_TEXT_LEFT );

		nk_spacing( pNkContext, 1 );

		nk_label( pNkContext, "Special Thanks:", NK_TEXT_LEFT );
		nk_label( pNkContext, "Remix Icon - https://remixicon.com/", NK_TEXT_LEFT );

		nk_spacing( pNkContext, 1 );
	}

	void ControllerUI::doDeviceUI( ImAppContext* pContext, ControllerState::ConnectedDevice& device )
	{
		nk_context* pNkContext = pContext->nkContext;

		const float deviceHeight = getDeviceHeight( pNkContext, device );
		nk_layout_row_dynamic( pNkContext, deviceHeight, 1 );

		if( !nk_group_begin( pNkContext, device.name.toConstCharPointer(), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR ) )
		{
			return;
		}

		// Title
		nk_layout_row_dynamic( pNkContext, 0.0f, 1 );
		{
			//nk_style_push_style_item( pNkContext, &pNkContext->style.window.fixed_background, pNkContext->style.window.header.normal );
			//CONCT_ASSERT( nk_panel_begin( pNkContext, "Bla", NK_PANEL_GROUP ) );

			//nk_layout_row_begin( pNkContext, NK_STATIC, 0.0f, 1 );
			//nk_label( pNkContext, "Bla", NK_TEXT_LEFT );

			//nk_panel_end( pNkContext );
			//nk_style_pop_style_item( pNkContext );

			UiHeader deviceHeader( pNkContext, "deviceHeader" );
			if( deviceHeader.isVisible )
			{
				const float width = nk_window_get_width( pNkContext );
				nk_layout_row_begin( pNkContext, NK_STATIC, 0.0f, 3 );

				nk_layout_row_push( pNkContext, 24.0f );
				nk_style_push_vec2( pNkContext, &pNkContext->style.window.spacing, nk_vec2( 0.0f, 9.0f ) );
				const struct nk_image instancesImage = ImAppImageGetBlocking( pContext, "icons/instances.png" );
				nk_image( pNkContext, instancesImage );
				nk_style_pop_vec2( pNkContext );

				const float headerPadding	= pNkContext->style.window.header.padding.x * 2.0f;
				const float imageWidth		= 24.0f + pNkContext->style.window.spacing.x + (pNkContext->style.window.group_padding.x * 2.0f);
				const float titleWidth		= pNkContext->style.window.group_padding.x * 2.0f;
				const float favWidth		= 24.0f + pNkContext->style.window.spacing.x + (pNkContext->style.window.group_padding.x * 2.0f);
				const float remainingWidth	= width - (headerPadding + imageWidth + titleWidth + favWidth);
				nk_layout_row_push( pNkContext, remainingWidth );
				nk_label( pNkContext, device.name.toConstCharPointer(), NK_TEXT_LEFT );

				// Favorite Button
				nk_layout_row_push( pNkContext, 24.0f + (pNkContext->style.button.padding.x * 2.0f) );
				{
					nk_style_push_float( pNkContext, &pNkContext->style.button.border, 0.0f );
					nk_style_push_float( pNkContext, &pNkContext->style.button.rounding, 0.0f );
					nk_style_push_vec2( pNkContext, &pNkContext->style.button.image_padding, nk_vec2( 0.0f, 0.0f ) );
					nk_style_push_style_item( pNkContext, &pNkContext->style.button.normal, pNkContext->style.window.header.normal );
					//nk_style_push_style_item( pNkContext, &pNkContext->style.button.active, pNkContext->style.window.header.active );
					nk_style_push_style_item( pNkContext, &pNkContext->style.button.hover, pNkContext->style.window.header.hover );

					const struct nk_image favImage = ImAppImageGetBlocking( pContext, "icons/fav-off.png" );
					if( nk_button_image( pNkContext, favImage ) )
					{

					}
					nk_style_pop_vec2( pNkContext );
					nk_style_pop_float( pNkContext );
					nk_style_pop_float( pNkContext );
					nk_style_pop_style_item( pNkContext );
					//nk_style_pop_style_item( pNkContext );
					nk_style_pop_style_item( pNkContext );
				}

				nk_layout_row_end( pNkContext );
			}
		}

		for( ControllerState::DeviceInstance& instance : device.instances )
		{
			doInstanceUI( pContext, device, instance );
		}

		nk_layout_row_static( pNkContext, 25.0f, 150, 2 );

		nk_button_label( pNkContext, "Edit" );
		nk_button_label( pNkContext, "Remove" );

		nk_group_end( pNkContext );
	}

	void ControllerUI::doInstanceUI( ImAppContext* pContext, const ControllerState::ConnectedDevice& device, ControllerState::DeviceInstance& instance )
	{
		nk_context* pNkContext = pContext->nkContext;

		const char* pTypeName = "Unknown Type";
		if( instance.pType != nullptr )
		{
			pTypeName = instance.pType->getName().toConstCharPointer();
		}

		{
			nk_layout_row_dynamic( pNkContext, 0.0f, 1 );

			const struct nk_image instanceImage = ImAppImageGetBlocking( pContext, "icons/instance.png" );
			UI::doHeaderImageLabel( pNkContext, instanceImage, pTypeName );

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
			doPropertyUI( pContext, device, instance, prop );
		}
	}

	void ControllerUI::doPropertyUI( ImAppContext* pContext, const ControllerState::ConnectedDevice& device, const ControllerState::DeviceInstance& instance, ControllerState::InstanceProperty& prop )
	{
		nk_context* pNkContext = pContext->nkContext;

		nk_layout_row_dynamic( pNkContext, 0.0f, 2 );

		nk_label( pNkContext, prop.pProperty->name.toConstCharPointer(), NK_TEXT_LEFT );

		if( prop.value.getType() == ValueType_Void &&
			prop.pGetCommand != nullptr )
		{
			nk_label( pNkContext, "Loading...", NK_TEXT_LEFT );
			return;
		}

		const ValueType valueType = prop.value.getType();
		switch( valueType )
		{
		case ValueType_Void:
			nk_label( pNkContext, "No Value", NK_TEXT_LEFT );
			break;

		case ValueType_Boolean:
			{
				bool active = prop.value.getBoolean();
				if( nk_checkbox_label( pNkContext, active ? "True" : "False", &active ) )
				{
					ValueHigh newValue;
					newValue.setBoolean( active );

					m_pState->changeProperty( device, instance, prop, newValue );
				}
			}
			break;

		case ValueType_Integer:
		case ValueType_Unsigned:
			{
				const sint64 value = valueType == ValueType_Integer ? prop.value.getInteger() : prop.value.getUnsigned();
				nk_labelf( pNkContext, NK_TEXT_LEFT, "%d", value );
			}
			break;

		case ValueType_PercentValue:
			{
				const int oldIntValue = (int)prop.value.getPercentValue();
				const int newIntValue = nk_slide_int( pNkContext, 0, oldIntValue, 65535, 65535 / 100 );
				if( newIntValue != oldIntValue )
				{
					ValueHigh newValue;
					newValue.setPercentValue( (PercentValue)newIntValue );

					m_pState->changeProperty( device, instance, prop, newValue );
				}
			}
			break;

		case ValueType_DeviceId:
		case ValueType_InstanceId:
		case ValueType_TypeCrc:
		case ValueType_String:
		case ValueType_Struct:
		case ValueType_Array:
			nk_label( pNkContext, "Not Implemented", NK_TEXT_LEFT );
			break;

		default:
			break;
		}
	}

	float ControllerUI::getDeviceHeight( nk_context* pNkContext, const ControllerState::ConnectedDevice& device ) const
	{
		float height = pNkContext->style.window.border;
		height += pNkContext->style.font->height;
		height += pNkContext->style.window.header.label_padding.y * 2.0f;
		height += pNkContext->style.window.header.padding.y * 2.0f;
		height += pNkContext->style.window.spacing.y * 2.0f;

		for( const ControllerState::DeviceInstance& instance : device.instances )
		{
			height += getInstanceHeight( pNkContext, instance );
		}

		return height;
	}

	float ControllerUI::getInstanceHeight( nk_context* pNkContext, const ControllerState::DeviceInstance& instance ) const
	{
		float height = pNkContext->style.window.group_border * 2.0f;
		height += pNkContext->style.font->height;
		height += pNkContext->style.window.header.label_padding.y * 2.0f;
		height += pNkContext->style.window.header.padding.y * 2.0f;
		height += pNkContext->style.window.spacing.y * 2.0f;

		for( const ControllerState::InstanceProperty& prop : instance.properties )
		{
			height += getPropertyHeight( pNkContext, prop );
		}

		return height;
	}

	float ControllerUI::getPropertyHeight( nk_context* pNkContext, const ControllerState::InstanceProperty& prop ) const
	{
		float height = pNkContext->style.font->height;
		height += pNkContext->style.window.padding.y * 2.0f;
		height += pNkContext->style.window.spacing.y * 2.0f;

		return height;
	}

	void ControllerUI::changeState( State state )
	{
		m_state = state;

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
