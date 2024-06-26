#pragma once

#include "conct_core.h"

#include <tiki/tiki_dynamic_array.h>
#include <tiki/tiki_flags.h>

namespace tinyxml2
{
	class XMLElement;
}

namespace conct
{
	enum class Capability : uint8
	{
		Allocation,
		I2C,
		GPIO,
		SPI,
		Sockets,
		UART,

		Count
	};

	using CapabilityFlags = Flags8< Capability >;

	class Capabilities
	{
		friend class Hardware;
		friend class Port;

	public:

		bool					hasCapability( Capability capability ) const { return m_capabilities.isSet( capability ); }
		const CapabilityFlags&	getCapabilities() const { return m_capabilities; }

		bool					metRequirements( const Capabilities& requirements, DynamicArray< Capability >* pMissingCapabilities = nullptr ) const;

	private: // friend

								Capabilities();

		bool					load( const tinyxml2::XMLElement* pNode );;

	private:

		CapabilityFlags			m_capabilities;
	};

	const char*					getCapabilityName( Capability value );
}