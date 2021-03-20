#pragma once

#include "conct_controller.h"
#include "conct_type_collection.h"
#include "conct_vector.h"

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
			uint16						port				= 0u;

			PortTcpClient*				pClientPort			= nullptr;
		};

		struct ConnectedDevice
		{
			using InstanceVector = Vector< DeviceInstance >;

			DeviceAddress				address;
			bool						allowRoutering		= false;
			DynamicString				name;
			uint32						serialNumber		= 0u;

			ValueCommand*				pNameCommand		= nullptr;
			ValueCommand*				pInstancesCommand	= nullptr;
			ValueCommand*				pDevicesCommand		= nullptr;

			InstanceVector				instances;
		};

		struct DeviceInstance
		{
			using PropertyVector = Vector< InstanceProperty >;

			Instance					instance			= { 0u, 0u };
			const InterfaceType*		pType				= nullptr;

			PropertyVector				properties;
		};

		struct InstanceProperty
		{
			const InterfaceProperty*	pProperty			= nullptr;

			ValueCommand*				pGetCommand			= nullptr;
			Command*					pSetCommand			= nullptr;

			ValueHigh					value;
			bool						hasValueChanged		= false;
		};

		using ConnectionVector = Vector< Connection* >;
		using DeviceVector = Vector< ConnectedDevice* >;

	public:

		bool							setup( ControllerConfig& config, RuntimeHigh& runtime );
		void							destroy();

		void							loop();

		const DeviceVector&				getDevices() const { return m_devices; }

		void							changeProperty( const ConnectedDevice& device, const DeviceInstance& instance, InstanceProperty& prop, const ValueHigh& value );

	private:

		RuntimeHigh*					m_pRuntime			= nullptr;

		ControllerConfig*				m_pConfig			= nullptr;
		uint32							m_configRevision	= (uint32)-1;

		Controller						m_controller;
		TypeCollection					m_types;

		ConnectionVector				m_connections;
		DeviceVector					m_devices;

		void							updateConfig();
		void							updateDevices();

		Connection*						createConnection( const DynamicString& hostname, uint16 port );
		void							destroyConnection( Connection* pConnection );

		ConnectedDevice*				createDevice( const DeviceAddress& address );
		void							destroyDevice( ConnectedDevice* pDevice );
		void							updateDevice( ConnectedDevice* pDevice );

		void							createInstance( DeviceInstance& instance, const Instance& data, const ConnectedDevice& device );
		void							destroyInstance( DeviceInstance& instance );
		void							updateInstance( DeviceInstance& instance, const ConnectedDevice& device );

		void							createProperty( InstanceProperty& prop, const InterfaceProperty& data, const ConnectedDevice& device, const DeviceInstance& instance );
		void							destroyProperty( InstanceProperty& prop );
		void							updateProperty( InstanceProperty& prop, const ConnectedDevice& device, const DeviceInstance& instance );
	};
}