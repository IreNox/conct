#pragma once

#include "conct_port.h"

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
#	include "conct_port_serial_linux.h"
#endif

namespace conct
{
#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
	struct PortSerialConfig
	{
		DynamicString portName;
	};
#endif

	class PortSerial : public Port
	{
	public:

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
		void						setConfig( const PortSerialConfig& config );
#endif

		virtual void				setup() CONCT_OVERRIDE_FINAL;
		virtual void				loop() CONCT_OVERRIDE_FINAL;

		virtual bool				openSend( Writer& writer, uintreg size, uintreg endpointId ) CONCT_OVERRIDE_FINAL;
		virtual void				closeSend( Writer& writer, uintreg endpointId ) CONCT_OVERRIDE_FINAL;

		virtual bool				openReceived( Reader& reader, uintreg& endpointId ) CONCT_OVERRIDE_FINAL;
		virtual void				closeReceived( Reader& reader, uintreg endpointId ) CONCT_OVERRIDE_FINAL;

		virtual Flags8< PortFlag >	getFlags() CONCT_OVERRIDE_FINAL;

	private:

		enum Type
		{
			Type_Hello,
			Type_Data,
			Type_Ack,

			Type_Count
		};

		enum Mode
		{
			Mode_Receive,
			Mode_SendAck,

			Mode_Send,
			Mode_WaitForAck,

			Mode_Count,
		};

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
		PortSerialConfig			m_config;

		Serial						Serial1;
#endif

		Mode						m_mode;
		uintreg						m_lastAckId;
		uintreg						m_counter;
		uintreg						m_remaining;
		uint8						m_headerBuffer[ 3u ];
		uint8						m_buffer[ 32u ];
	};
}