#pragma once

#include "conct_core.h"

#include <tiki/tiki_array_view.h>
#include <tiki/tiki_flags.h>

namespace conct
{
	class BinaryReader;
	class BinaryWriter;

	enum PortFlag
	{
		PortFlag_Client			= 1u << 0u,
		PortFlag_Server			= 1u << 1u,
		PortFlag_Reliable		= 1u << 2u
	};

	class Port
	{
	public:

		virtual void				getEndpoints( ConstArrayView< uintreg >& endpoints ) = 0;
		virtual bool				popConnectionReset( uintreg& endpointId ) = 0;

		virtual void				loop() = 0;

		virtual bool				openSend( BinaryWriter& writer, uintreg size, uintreg endpointId ) = 0;
		virtual void				closeSend( BinaryWriter& writer, uintreg endpointId ) = 0;

		virtual bool				openReceived( BinaryReader& reader, uintreg& endpointId ) = 0;
		virtual void				closeReceived( BinaryReader& reader, uintreg endpointId ) = 0;

		virtual Flags8< PortFlag >	getFlags() const = 0;
	};
}