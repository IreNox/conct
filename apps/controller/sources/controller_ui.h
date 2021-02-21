#pragma once

struct ImAppContext;

namespace conct
{
	class ControllerConfig;

	class ControllerUI
	{
	public:

		void				setup( ControllerConfig* pConfig );

		void				doUI( const ImAppContext* pContext );

	private:

		enum class State
		{
			Home,
			Devices,
			DeviceInstances,
			Connections,
			ConnectionEdit,
			Credits
		};

		ControllerConfig*	m_pConfig				= nullptr;

		State				m_state					= State::Connections;
		bool				m_isMenuOpen			= false;

		uintreg				m_selectedConnection	= 0u;

		DynamicString		m_editTitle;
		DynamicString		m_editHostname;
		DynamicString		m_editPort;

		void				doMenuUI( const ImAppContext* pContext );

		void				doHomeUI( const ImAppContext* pContext );
		void				doDevicesUI( const ImAppContext* pContext );
		void				doDeviceInstancesUI( const ImAppContext* pContext );
		void				doConnectionsUI( const ImAppContext* pContext );
		void				doConnectionEditUI( const ImAppContext* pContext );
		void				doCreditsUI( const ImAppContext* pContext );

		void				changeState( State state );

		static const char*	getStateTitle( State value );
	};
}