#pragma once

#include "console_plugin.h"

#include "conct_core.h"
#include "conct_dynamic_string.h"
#include "conct_runtime.h"
#include "conct_structs.h"
#include "conct_timer.h"
#include "conct_value_high.h"

#include <vector>
#include <forward_list>

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
			//State_Device,
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

			Action_GetProperty,
			Action_SetProperty,
			Action_CallFunction,

			Action_Count
		};

		struct ControllerDevice
		{
			DynamicString			name;
			DeviceAddress			address;
		};

		struct ControllerInstance
		{
			DynamicString			name;
			ControllerDevice*		pDevice;
			InstanceId				instanceId;
			const InterfaceType*	pType;
		};

		typedef std::vector< DynamicString > StringVector;
		typedef std::vector< State > StateVector;
		typedef std::forward_list< ControllerDevice > ControllerDeviceList;
		typedef std::forward_list< ControllerInstance > ControllerInstanceList;

		TypeCollection*							m_pTypes;

		State									m_state;
		StateVector								m_stateHistory;
		Action									m_action;
		uintreg									m_index;
		uintreg									m_lastIndices[ State_Count ];
		StringVector							m_list;
		DynamicString							m_valueText;
		DynamicString							m_popupText;
		ValueType								m_valueType;

		const ControllerDevice*					m_pDevice;
		const ControllerInstance*				m_pInstance;
		const InterfaceType*					m_pInterface;
		const InterfaceProperty*				m_pProperty;
		const InterfaceFunction*				m_pFunction;
		std::vector< ValueHigh >				m_values;

		ControllerDeviceList					m_devices;
		ControllerInstanceList					m_instances;

		CommandBase*							m_pRunningCommand;

		Timer									m_timer;

		void									updateList( ConsoleDevice& device );
		void									updateValue( ConsoleDevice& device );

		void									previousState();
		void									nextState( ConsoleDevice& device );
		void									setState( State state );
		void									setValueState( ValueType targetType );
		void									setPopupState( const DynamicString& text );

		bool									setValueFromString( ValueHigh& value, const DynamicString& text );
		DynamicString							getStringFromValue( const ValueHigh& value );

		DynamicString							addInstance( Instance sourceInstance );

		void									drawClear() const;
		void									drawList() const;
		void									drawValue() const;
		void									drawLoading() const;
		void									drawPopup() const;

		void									buildActions();
		//void									buildDevices();
		void									buildInstances();
		void									buildTypes();
		void									buildProperties();
		void									buildFunctions();

		void									executeAction( ConsoleDevice& device );
		void									executeGetPropertyAction( ConsoleDevice& device, const RemoteInstance& remoteInstance );
		void									executeSetPropertyAction( ConsoleDevice& device, const RemoteInstance& remoteInstance );
		void									executeCallFunctionAction( ConsoleDevice& device, const RemoteInstance& remoteInstance );

		void									finishCommand( ConsoleDevice& device );
	};
}