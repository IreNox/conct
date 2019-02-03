#pragma once

#include "conct_unsorted_set.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace conct
{
	enum Capability
	{
		Capability_Allocation,
		Capability_I2C,
		Capability_GPIO,
		Capability_SPI,
		Capability_Sockets,
		Capability_UART,

		Capability_Count
	};

	class Capabilities
	{
		friend class Hardware;
		friend class Port;

	public:

		bool								hasCapability( Capability capability ) const { return m_capabilities.contains( capability ); }
		const UnsortedSet< Capability >&	getCapabilities() const { return m_capabilities; }

		bool								metRequirements( const Capabilities& requirements, UnsortedSet< Capability >* pMissingCapabilities = nullptr ) const;

	private: // friend

											Capabilities();

		bool								load( const tinyxml2::XMLElement* pNode );;

	private:

		UnsortedSet< Capability >			m_capabilities;
	};

	const char*								getCapabilityName( Capability value );
}