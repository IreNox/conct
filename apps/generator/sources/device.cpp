#include "device.h"

#include "conct_interface_type.h"
#include "conct_path.h"
#include "conct_string_tools.h"
#include "conct_trace.h"
#include "conct_type_collection.h"
#include "conct_xml_helper.h"

#include "hardware.h"
#include "hardware_collection.h"
#include "port.h"
#include "port_collection.h"

#include <tinyxml2/tinyxml2.h>

namespace conct
{
	Device::Device()
	{
		m_pHardware = nullptr;

	}

	bool Device::load( const Path& fileName, HardwareCollection& hardwareCollection, PortCollection& portCollection, TypeCollection& typeCollection )
	{
		tinyxml2::XMLDocument document;
		if( !loadDocument( document, fileName ) )
		{
			return false;
		}

		tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "device" );
		if( pRootNode == nullptr )
		{
			traceNodeError( pRootNode, "Error: Failed to find 'device' root node.\n" );
			return false;
		}

		if( !loadStringValue( m_name, pRootNode, "name" ) )
		{
			return false;
		}

		DynamicString hardwareName;
		if( !loadStringValue( hardwareName, pRootNode, "hardware" ) )
		{
			return false;
		}

		m_pHardware = hardwareCollection.findHardwareByName( hardwareName );
		if( m_pHardware == nullptr )
		{
			traceNodeError( pRootNode, "Error: Failed to find hardware with name '"_s + hardwareName + "'.\n" );
			return false;
		}

		tinyxml2::XMLElement* pPortsNode = pRootNode->FirstChildElement( "ports" );
		if( pPortsNode == nullptr )
		{
			traceNodeError( pRootNode, "Error: Failed to find 'ports' node.\n" );
			return false;
		}

		for( tinyxml2::XMLElement* pPortNode = pPortsNode->FirstChildElement( "port" ); pPortNode != nullptr; pPortNode = pPortNode->NextSiblingElement( "port" ) )
		{
			DevicePort port;

			DynamicString portType;
			if( !loadStringValue( portType, pPortNode, "type" ) ||
				!loadMemSizeValue( port.index, pPortNode, "index" ) )
			{
				return false;
			}

			port.pPort = portCollection.findPortByName( portType );
			if( port.pPort == nullptr )
			{
				traceNodeError( pPortNode, "Error: Failed to find port with tag '"_s + portType + "'.\n" );
				return false;
			}

			UnsortedSet< Capability > missingCapabilities;
			if( !m_pHardware->getCapabilities().metRequirements( port.pPort->getRequirements(), &missingCapabilities ) )
			{
				DynamicString missing;
				for( Capability c : missingCapabilities )
				{
					if( !missing.isEmpty() )
					{
						missing += ", ";
					}

					missing += getCapabilityName( c );
				}

				traceNodeError( pPortNode, "Error: The requirements for Port '"_s + portType + "' aren't met. Missing: " + missing + " \n" );
				return false;
			}

			const Port::ParameterMap& parameters = port.pPort->getParameters();

			const tinyxml2::XMLElement* pPortParametersNode = pPortNode->FirstChildElement( "parameters" );
			if( pPortParametersNode != nullptr )
			{
				for( const tinyxml2::XMLElement* pPortParameterNode = pPortParametersNode->FirstChildElement( "parameter" ); pPortParameterNode != nullptr; pPortParameterNode = pPortParameterNode->NextSiblingElement( "parameter" ) )
				{
					DynamicString nameString;
					DynamicString valueString;
					if( !loadStringValue( nameString, pPortParameterNode, "name" ) ||
						!loadStringValue( valueString, pPortParameterNode, "value" ) )
					{
						traceNodeError( pPortParameterNode, "Error: could not find port parameter name or value attribute.\n" );
						return false;
					}

					const PortParameter* pParameter = parameters[ nameString ];
					if( pParameter == nullptr )
					{
						traceNodeError( pPortParameterNode, "Error: could not find port parameter with name '"_s + nameString + "'.\n" );
						return false;
					}

					PortParameterValue value;
					if( !pParameter->loadValue( value, valueString.toConstCharPointer() ) )
					{
						traceNodeError( pPortParameterNode, "Error: value '"_s + valueString + "' is not valid for port parameter '" + nameString + "'.\n" );
						return false;
					}

					port.parameters[ pParameter ] = value;
				}
			}

			for( const Port::ParameterMap::PairType& kvp : parameters )
			{
				const PortParameter* pParameter = kvp.value;
				if( pParameter->getDefaultValue().type != PortParameterValueType_Invalid ||
					!pParameter->matchFilters( *this ) )
				{
					continue;
				}

				const bool hasParameter = port.parameters.hasKey( pParameter );
				if( !hasParameter )
				{
					traceNodeError( pPortNode, "Error: port "_s + string_tools::toString( port.index ) + " doesn't set required parameter '" + pParameter->getName() + "'.\n" );
					return false;
				}
			}

			m_ports.pushBack( port );
		}

		m_dynamicPorts = false;
		loadBooleanValue( m_dynamicPorts, pPortsNode, "dynamic" );

		if( m_ports.isEmpty() && !m_dynamicPorts )
		{
			traceNodeError( pRootNode, "Error: Device has no ports.\n" );
			return false;
		}

		tinyxml2::XMLElement* pInterfacesNode = pRootNode->FirstChildElement( "interfaces" );
		if( pInterfacesNode != nullptr )
		{
			for( tinyxml2::XMLElement* pInterfaceNode = pInterfacesNode->FirstChildElement( "interface" ); pInterfaceNode != nullptr; pInterfaceNode = pInterfaceNode->NextSiblingElement( "interface" ) )
			{
				const InterfaceType* pInterface = nullptr;
				if( !loadInterfaceValue( &pInterface, pInterfaceNode, "type", ""_s, typeCollection ) )
				{
					return false;
				}

				m_interfaces.insert( pInterface );
			}
		}

		DeviceInstance deviceInstance;
		deviceInstance.id = m_instances.getLength();
		deviceInstance.pInterface = typeCollection.findInterface( "Core.Device"_s, ""_s );
		m_instances.pushBack( deviceInstance );
		m_interfaces.insert( deviceInstance.pInterface );
		m_proxies.insert( deviceInstance.pInterface );

		if( needsRouter() )
		{
			DeviceInstance routerInstance;
			routerInstance.id = m_instances.getLength();
			routerInstance.pInterface = typeCollection.findInterface( "Core.Router"_s, ""_s );
			m_instances.pushBack( routerInstance );
			m_interfaces.insert( routerInstance.pInterface );
			m_proxies.insert( routerInstance.pInterface );
		}

		tinyxml2::XMLElement* pInstancesNode = pRootNode->FirstChildElement( "instances" );
		if( pInstancesNode != nullptr )
		{
			for( tinyxml2::XMLElement* pInstanceNode = pInstancesNode->FirstChildElement( "instance" ); pInstanceNode != nullptr; pInstanceNode = pInstanceNode->NextSiblingElement( "instance" ) )
			{
				DeviceInstance instance;
				instance.id = m_instances.getLength();

				if( !loadInterfaceValue( &instance.pInterface, pInstanceNode, "type", ""_s, typeCollection ) )
				{
					return false;
				}

				if( instance.pInterface->getSingleton() )
				{
					for( const DeviceInstance& instance2 : m_instances )
					{
						if( instance.pInterface == instance2.pInterface )
						{
							traceNodeError( pInstanceNode, "Error: Device has more than one instance of a singleton interface. Type name: "_s + instance.pInterface->getFullName() + "\n" );
							return false;
						}
					}
				}

				m_instances.pushBack( instance );
				m_interfaces.insert( instance.pInterface );
				m_proxies.insert( instance.pInterface );
			}
		}

		return true;
	}

	bool Device::needsRouter() const
	{
		if( m_ports.getLength() > 1u )
		{
			return true;
		}

		for( const DevicePort& port : m_ports )
		{
			if( port.pPort->getMultiConnection() )
			{
				return true;
			}
		}

		return m_pHardware->getRuntime() == HardwareRuntime_High;
	}
}
