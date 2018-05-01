#pragma once

#include "console_plugin.h"

#include "conct_core.h"
#include "conct_runtime.h"
#include "conct_structs.h"
#include "conct_timer.h"

#include <string>
#include <vector>
#include <deque>

namespace conct
{
	class CommandBase;
	class InterfaceType;
	class TypeCollection;
	struct InterfaceFunction;
	struct InterfaceProperty;

	class ConsoleController : public ConsolePlugin
	{
	public:

									ConsoleController( TypeCollection* pTypes );

		virtual void				activate( ConsoleDevice& device ) CONCT_OVERRIDE_FINAL;
		virtual void				deactivate( ConsoleDevice& device ) CONCT_OVERRIDE_FINAL;

		virtual void				update( ConsoleDevice& device ) CONCT_OVERRIDE_FINAL;
		virtual void				draw( const ConsoleDevice& device ) const CONCT_OVERRIDE_FINAL;

		virtual const char*			getName() const CONCT_OVERRIDE_FINAL;

	private:

		enum State
		{
			State_Invalid = -1,

			State_Action,
			State_Device,
			State_Instance,
			State_Type,
			State_Property,
			State_Function,
			State_Value,
			State_Waiting,
			State_Popup,

			State_Count
		};

		enum Action
		{
			Action_Invalid = -1,

			Action_GetInstance,
			Action_GetProperty,
			Action_SetProperty,
			Action_CallFunction,

			Action_Count
		};

		struct ControllerDevice
		{
			std::string				name;
			DeviceAddress			address;
		};

		struct ControllerInstance
		{
			std::string				name;
			TypeCrc					type;
			RemoteInstance			instance;
		};

		typedef std::vector< std::string > StringVector;
		typedef std::vector< State > StateVector;
		typedef std::vector< ControllerDevice > ControllerDeviceVector;
		typedef std::vector< ControllerInstance > ControllerInstanceVector;

		TypeCollection*							m_pTypes;

		State									m_state;
		StateVector								m_stateHistory;
		Action									m_action;
		uintreg									m_index;
		uintreg									m_lastIndices[ State_Count ];
		StringVector							m_list;
		std::string								m_valueText;
		std::string								m_popupText;

		const ControllerDevice*					m_pDevice;
		const ControllerInstance*				m_pInstance;
		const InterfaceType*					m_pInterface;
		const InterfaceProperty*				m_pProperty;
		const InterfaceFunction*				m_pFunction;
		std::vector< Value >					m_values;

		ControllerDeviceVector					m_devices;
		ControllerInstanceVector				m_instances;

		CommandBase*							m_pRunningCommand;

		Timer									m_timer;

		void									updateList( ConsoleDevice& device );
		void									updateValue( ConsoleDevice& device );

		void									previousState();
		void									nextState( ConsoleDevice& device );
		void									setState( State state );
		void									showPopup( const std::string& text );

		bool									setValueFromString( Value& value, const std::string& text );

		void									drawClear() const;
		void									drawList() const;
		void									drawValue() const;
		void									drawLoading() const;
		void									drawPopup() const;

		void									buildActions();
		void									buildDevices();
		void									buildInstances();
		void									buildTypes();
		void									buildProperties();
		void									buildFunctions();

		void									executeAction( ConsoleDevice& device );
		void									executeGetInstanceAction( ConsoleDevice& device );
		void									executeGetPropertyAction( ConsoleDevice& device );
		void									executeSetPropertyAction( ConsoleDevice& device );
		void									executeCallFunctionAction( ConsoleDevice& device );

		void									finishCommand();
	};
}