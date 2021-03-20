#pragma once

struct ImAppContext;
struct nk_context;

namespace conct
{
	class ControllerConfig;

	class ControllerUI
	{
	public:

		void				setup( ControllerConfig& config, ControllerState& state );

		void				doUI( ImAppContext* pContext );

	private:

		enum class State
		{
			Home,
			Devices,
			Connections,
			ConnectionEdit,
			Credits
		};

		ControllerConfig*	m_pConfig				= nullptr;
		ControllerState*	m_pState				= nullptr;

		State				m_state					= State::Devices;
		bool				m_isMenuOpen			= false;

		uintreg				m_selectedConnection	= 0u;

		DynamicString		m_editTitle;
		DynamicString		m_editHostname;
		DynamicString		m_editPort;

		void				doMenuUI( ImAppContext* pContext );

		void				doHomeUI( ImAppContext* pContext );
		void				doDevicesUI( ImAppContext* pContext );
		void				doConnectionsUI( ImAppContext* pContext );
		void				doConnectionEditUI( ImAppContext* pContext );
		void				doCreditsUI( ImAppContext* pContext );

		void				doDeviceUI( ImAppContext* pContext, ControllerState::ConnectedDevice& device );
		void				doInstanceUI( ImAppContext* pContext, const ControllerState::ConnectedDevice& device, ControllerState::DeviceInstance& instance );
		void				doPropertyUI( ImAppContext* pContext, const ControllerState::ConnectedDevice& device, const ControllerState::DeviceInstance& instance, ControllerState::InstanceProperty& prop );

		float				getDeviceHeight( nk_context* pNkContext, const ControllerState::ConnectedDevice& device ) const;
		float				getInstanceHeight( nk_context* pNkContext, const ControllerState::DeviceInstance& instance ) const;
		float				getPropertyHeight( nk_context* pNkContext, const ControllerState::InstanceProperty& prop ) const;

		void				changeState( State state );

		static const char*	getStateTitle( State value );
		static const char*	getStateIcon( State value );
	};
}