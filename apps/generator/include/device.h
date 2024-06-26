#pragma once

#include "conct_dynamic_string.h"
#include "conct_map.h"
#include "conct_unsorted_set.h"
#include "conct_vector.h"

#include "port_parameter.h"

namespace conct
{
	class Hardware;
	class HardwareCollection;
	class InterfaceType;
	class Path;
	class Port;
	class PortCollection;
	class StructType;
	class TypeCollection;

	struct DevicePort
	{
		typedef Map< const PortParameter*, PortParameterValue > ParameterValueMap;

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

		typedef Vector< DevicePort > PortVector;
		typedef UnsortedSet< const InterfaceType* > InterfaceSet;
		typedef UnsortedSet< const StructType* > StructSet;
		typedef Vector< DeviceInstance > InstanceVector;

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

		const Hardware*			m_pHardware;

		bool					m_dynamicPorts;
		PortVector				m_ports;

		InterfaceSet			m_interfaces;
		StructSet				m_structs;

		InstanceVector			m_instances;
		InterfaceSet			m_proxies;
	};
}