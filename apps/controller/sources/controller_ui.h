#pragma once

struct ImAppContext;

namespace conct
{
	class ControllerConfig;

	class ControllerUI
	{
	public:

		void				setup( ControllerConfig* pConfig );

		void				doUI( ImAppContext* pContext );

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

		void				doMenuUI( ImAppContext* pContext );

		void				doHomeUI( ImAppContext* pContext );
		void				doDevicesUI( ImAppContext* pContext );
		void				doDeviceInstancesUI( ImAppContext* pContext );
		void				doConnectionsUI( ImAppContext* pContext );
		void				doConnectionEditUI( ImAppContext* pContext );
		void				doCreditsUI( ImAppContext* pContext );

		void				changeState( State state );

		static const char*	getStateTitle( State value );
		static const char*	getStateIcon( State value );
	};
}