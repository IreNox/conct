#pragma once

#include "port_parameter.h"

#include "tiki/tiki_hash_set.h"
#include "tiki/tiki_hash_map.h"

namespace conct
{
	class Hardware;
	class HardwareCollection;
	class InterfaceType;
	class Port;
	class PortCollection;
	class StructType;
	class TypeCollection;

	struct DevicePort
	{
		typedef HashMap< const PortParameter*, PortParameterValue > ParameterValueMap;

		size_t				index;
		const Port*			pPort;
		ParameterValueMap	parameters;
	};

	struct DeviceInstance
	{
		size_t					id;
		const InterfaceType*	pInterface;
	};

	class Device
	{
	public:

		typedef DynamicArray< DevicePort > PortVector;
		typedef HashSet< const InterfaceType* > InterfaceSet;
		typedef HashSet< const StructType* > StructSet;
		typedef DynamicArray< DeviceInstance > InstanceVector;

								Device();

		bool					load( const Path& fileName, HardwareCollection& hardwareCollection, PortCollection& portCollection, TypeCollection& typeCollection );

		const DynamicString&	getName() const { return m_name; }

		const Hardware&			getHardware() const { return *m_pHardware; }
		const PortVector&		getPorts() const { return m_ports; }

		const InterfaceSet&		getInterfaces() const { return m_interfaces; }
		const StructSet&		getStructs() const { return m_structs; }

		const InstanceVector&	getInstances() const { return m_instances; }
		const InterfaceSet&		getProxies() const { return m_proxies; }

		bool					needsRouter() const;

	private:

		DynamicString			m_name;

		const Hardware*			m_pHardware		= nullptr;
		bool					m_router		= false;

		bool					m_dynamicPorts	= false;
		PortVector				m_ports;

		InterfaceSet			m_interfaces;
		StructSet				m_structs;

		InstanceVector			m_instances;
		InterfaceSet			m_proxies;
	};
}