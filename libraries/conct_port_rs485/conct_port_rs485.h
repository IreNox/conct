#pragma once

#include "conct_port.h"

namespace conct
{
	class PortRs485 : public Port
	{
	public:

		virtual void	setup() CONCT_OVERRIDE_FINAL;
		virtual void	loop() CONCT_OVERRIDE_FINAL;

		virtual bool	openSend( Writer& writer, uint8_t deviceId ) CONCT_OVERRIDE_FINAL;
		virtual void	closeSend() CONCT_OVERRIDE_FINAL;

		virtual bool	openReceived( Reader& reader, uint8_t deviceId ) CONCT_OVERRIDE_FINAL;
		virtual void	closeReceived() CONCT_OVERRIDE_FINAL;

	private:

		uint8_t			m_buffer[ 64u ];
	};
}