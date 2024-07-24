#pragma once

struct ImAppContext;
struct ImUiSurface;

namespace imui
{
	class UiSurface;
	class UiWindow;

	namespace toolbox
	{
		class UiToolboxWindow;
	}
}

namespace conct
{
	using namespace imui;
	using namespace imui::toolbox;

	class ControllerConfig;

	class ControllerUI
	{
	public:

		void				setup( ControllerConfig& config, ControllerState& state );

		void				doUI( ImAppContext* context, ImUiSurface* surface );

	private:

		enum class State
		{
			Home,
			Devices,
			Connections,
			ConnectionEdit,
			Credits
		};

		ControllerConfig*	m_config				= nullptr;
		ControllerState*	m_state					= nullptr;

		State				m_menuState				= State::Devices;
		bool				m_isMenuOpen			= false;

		uintreg				m_selectedConnection	= 0u;

		DynamicString		m_editTitle;
		DynamicString		m_editHostname;
		DynamicString		m_editPort;

		void				doMenuUI( ImAppContext* context, UiSurface& surface, float menuWidth );

		void				doHomeUI( UiToolboxWindow& window );
		void				doDevicesUI( UiToolboxWindow& window );
		void				doConnectionsUI( UiToolboxWindow& window );
		void				doConnectionEditUI( UiToolboxWindow& window );
		void				doCreditsUI( UiToolboxWindow& window );

		void				doDeviceUI( UiToolboxWindow& window, ControllerState::ConnectedDevice& device );
		void				doInstanceUI( UiToolboxWindow& window, const ControllerState::ConnectedDevice& device, ControllerState::DeviceInstance& instance );
		void				doPropertyUI( UiToolboxWindow& window, const ControllerState::ConnectedDevice& device, const ControllerState::DeviceInstance& instance, ControllerState::InstanceProperty& prop );

		//float				getDeviceHeight( nk_context* pNkContext, const ControllerState::ConnectedDevice& device ) const;
		//float				getInstanceHeight( nk_context* pNkContext, const ControllerState::DeviceInstance& instance ) const;
		//float				getPropertyHeight( nk_context* pNkContext, const ControllerState::InstanceProperty& prop ) const;

		void				changeState( State state );

		static const char*	getStateTitle( State value );
		static const char*	getStateIcon( State value );
	};
}