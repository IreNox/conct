#pragma once

#include "base.h"

#include <set>

namespace conct
{
	class Hardware;
	class HardwareCollection;
	class Interface;
	class Port;
	class PortCollection;
	class TypeCollection;

	struct DevicePort
	{
		size_t		index;
		const Port*	pPort;
	};

	struct DeviceInstance
	{
		size_t				id;
		const Interface*	pInterface;
	};

	class Device
	{
	public:

		typedef std::vector< DevicePort > PortVector;
		typedef std::set< const Interface* > InterfaceSet;
		typedef std::vector< DeviceInstance > InstanceVector;

								Device();
		bool					load( const std::string& fileName, HardwareCollection& hardwareCollection, PortCollection& portCollection, TypeCollection& typeCollection );

		const std::string&		getName() const { return m_name; }

		const Hardware&			getHardware() const { return *m_pHardware; }
		const PortVector&		getPorts() const { return m_ports; }
		const InterfaceSet&		getInterfaces() const { return m_interfaces; }
		const InstanceVector&	getInstances() const { return m_instances; }

	private:

		std::string				m_name;

		const Hardware*			m_pHardware;
		PortVector				m_ports;
		InterfaceSet			m_interfaces;
		InstanceVector			m_instances;
	};
}