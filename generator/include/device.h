#pragma once

#include "base.h"

#include <set>

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
		size_t		index;
		const Port*	pPort;
	};

	struct DeviceInstance
	{
		size_t					id;
		const InterfaceType*	pInterface;
	};

	class Device
	{
	public:

		typedef std::vector< DevicePort > PortVector;
		typedef std::set< const InterfaceType* > InterfaceSet;
		typedef std::set< const StructType* > StructSet;
		typedef std::vector< DeviceInstance > InstanceVector;

								Device();
		bool					load( const std::string& fileName, HardwareCollection& hardwareCollection, PortCollection& portCollection, TypeCollection& typeCollection );

		const std::string&		getName() const { return m_name; }

		const Hardware&			getHardware() const { return *m_pHardware; }
		const PortVector&		getPorts() const { return m_ports; }

		const InterfaceSet&		getInterfaces() const { return m_interfaces; }
		const StructSet&		getStructs() const { return m_structs; }

		const InstanceVector&	getInstances() const { return m_instances; }
		const InterfaceSet&		getProxies() const { return m_proxies; }

	private:

		std::string				m_name;

		const Hardware*			m_pHardware;
		PortVector				m_ports;
		InterfaceSet			m_interfaces;
		StructSet				m_structs;
		InstanceVector			m_instances;
		InterfaceSet			m_proxies;
	};
}