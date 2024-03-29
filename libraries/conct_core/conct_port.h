#pragma once

#include "conct_array_view.h"
#include "conct_core.h"
#include "conct_flags.h"

namespace conct
{
	class Reader;
	class Writer;

	enum PortFlag
	{
		PortFlag_Client			= 1u << 0u,
		PortFlag_Server			= 1u << 1u,
		PortFlag_Reliable		= 1u << 2u
	};

	class Port
	{
	public:

		virtual void				getEndpoints( ArrayView< uintreg >& endpoints ) = 0;
		virtual bool				popConnectionReset( uintreg& endpointId ) = 0;

		virtual void				loop() = 0;

		virtual bool				openSend( Writer& writer, uintreg size, uintreg endpointId ) = 0;
		virtual void				closeSend( Writer& writer, uintreg endpointId ) = 0;

		virtual bool				openReceived( Reader& reader, uintreg& endpointId ) = 0;
		virtual void				closeReceived( Reader& reader, uintreg endpointId ) = 0;

		virtual Flags8< PortFlag >	getFlags() const = 0;
	};
}