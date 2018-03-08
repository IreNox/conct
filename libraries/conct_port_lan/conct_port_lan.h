#pragma once

#include "conct_port.h"

namespace conct
{
	class PortLan : public Port
	{
	public:

		virtual void	setup() CONCT_OVERRIDE_FINAL;
		virtual void	loop() CONCT_OVERRIDE_FINAL;

		virtual bool	openSend( Writer& writer, muint size, DeviceId deviceId ) CONCT_OVERRIDE_FINAL;
		virtual void	closeSend( Writer& writer ) CONCT_OVERRIDE_FINAL;

		virtual bool	openReceived( Reader& reader, DeviceId& deviceId ) CONCT_OVERRIDE_FINAL;
		virtual void	closeReceived( Reader& reader ) CONCT_OVERRIDE_FINAL;

	private:

	};
}