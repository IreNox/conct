#pragma once

#include "conct_port.h"

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
#	include "conct_dynamic_string.h"
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

		enum Type : uint8
		{
			Type_Hello,
			Type_Data,
			Type_Ack,

			Type_Count
		};

		enum State
		{
			State_Idle,
			State_ReceivingHeader,
			State_ReceivingData,
			State_ReceivedPacket,
			State_Send,
			State_WaitingForAck
		};

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
		PortSerialConfig			m_config;

		Serial						Serial1;
#endif

		State						m_state;
		uintreg						m_lastSendId;
		uintreg						m_counter;
		uint8						m_headerBuffer[ 3u ];
		uint8						m_buffer[ 31u ];

		uint16						getMagicFromHeader() const;
		uint8						getSizeFromHeader() const;
		Type						getTypeFromHeader() const;
		uint8						getIdFromHeader() const;

		void						writeHeader( uint8 size, Type type, uint8 id );

		bool						updateReceiveHeader();
		bool						updateReceiveData();
		bool						updateSend();
		bool						updateWaitForAck();

		void						sendHello();
		void						sendAck( uintreg packetId );

		uint8						calculateChecksum() const;
	};
}