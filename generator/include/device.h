#pragma once

#include "base.h"

namespace conct
{
	class Hardware;
	class HardwareCollection;
	class Interface;
	class Service;
	class ServiceCollection;
	class TypeCollection;

	class Device
	{
	public:

		typedef std::vector< const Service* > ServiceVector;
		typedef std::vector< const Interface* > InterfaceVector;

								Device();
		bool					load( const std::string& fileName, HardwareCollection& hardwareCollection, ServiceCollection& serviceCollection, TypeCollection& typeCollection );

		const Hardware&			getHardware() const { return *m_pHardware; }
		const ServiceVector&	getServices() const { return m_services; }
		const InterfaceVector&	getInterfaces() const { return m_interfaces; }

	private:

		const Hardware*			m_pHardware;
		ServiceVector			m_services;
		InterfaceVector			m_interfaces;
	};
}