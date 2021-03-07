#pragma once

#include "conct_vector.h"
#include "conct_controller.h"

#include "controller_config.h"

namespace conct
{
	class ControllerConfig;
	class Device;
	class PortTcpClient;
	class Type;
	struct InterfaceProperty;

	class ControllerState
	{
	public:

		struct ConnectedDevice;
		struct DeviceInstance;
		struct InstanceProperty;

		struct Connection
		{
			DynamicString				hostname;
			uint16						port		= 0u;

			PortTcpClient*				pClientPort	= nullptr;
		};

		struct ConnectedDevice
		{
			using InstanceVector = Vector< DeviceInstance >;

			DeviceAddress				address;
			DynamicString				name;
			uint32						serialNumber		= 0u;

			Command*					pNameCommand		= nullptr;
			Command*					pInstancesCommand	= nullptr;
			Command*					pDevicesCommand		= nullptr;

			InstanceVector				instances;
		};

		struct DeviceInstance
		{
			using PropertyVector = Vector< InstanceProperty >;

			Instance					instance			= { 0u, 0u };
			const Type*					pType				= nullptr;

			PropertyVector				properties;
		};

		struct InstanceProperty
		{
			const InterfaceProperty*	pProperty			= nullptr;

			Command*					pGetCommand			= nullptr;
			Command*					pSetCommand			= nullptr;

			ValueHigh					value;
			bool						hasValueChanged		= false;
		};

		void							setup( ControllerConfig& config, RuntimeHigh& runtime );
		void							destroy();

		void							loop();

	private:

		using ConnectionVector = Vector< Connection* >;
		using DeviceVector = Vector< ConnectedDevice* >;

		RuntimeHigh*					m_pRuntime			= nullptr;

		ControllerConfig*				m_pConfig			= nullptr;
		uint32							m_configRevision	= (uint32)-1;

		Controller						m_controller;

		ConnectionVector				m_connections;
		DeviceVector					m_devices;

		void							updateConfig();
		void							updateDevices();

		Connection*						createConnection( const DynamicString& hostname, uint16 port );
		void							destroyConnection( Connection* pConnection );

		ConnectedDevice*				createDevice( const DeviceAddress& address );
		void							destroyDevice( ConnectedDevice* pDevice );

		void							destroyInstance( DeviceInstance* pInstance );
		void							destroyProperty( InstanceProperty* pProperty );
	};
}