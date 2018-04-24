#pragma once

#include "conct_core.h"
#include "conct_flags.h"

namespace conct
{
	class Reader;
	class Writer;

	enum PortFlag
	{
		PortFlag_SingleEndpoint,
		PortFlag_MultiEndpoint,
		PortFlag_Reliable
	};

	class Port
	{
	public:

		virtual void				setup() = 0;
		virtual void				loop() = 0;

		virtual bool				openSend( Writer& writer, uintreg size, DeviceId deviceId ) = 0;
		virtual void				closeSend( Writer& writer ) = 0;

		virtual bool				openReceived( Reader& reader, DeviceId& deviceId ) = 0;
		virtual void				closeReceived( Reader& reader ) = 0;

		virtual Flags8< PortFlag >	getFlags() = 0;
	};
}