#pragma once

#include "conct_core.h"

namespace conct
{
	class Reader;
	class Writer;

	class Port
	{
	public:

		virtual void		setup() = 0;
		virtual void		loop() = 0;

		virtual bool		openSend( Writer& writer, uintreg size, DeviceId deviceId ) = 0;
		virtual void		closeSend( Writer& writer ) = 0;

		virtual bool		openReceived( Reader& reader, DeviceId& deviceId ) = 0;
		virtual void		closeReceived( Reader& reader ) = 0;
	};
}