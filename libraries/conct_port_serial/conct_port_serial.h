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

		bool						popConnectionReset( uintreg& endpointId );

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

		enum State : uint8
		{
			State_Idle,
			State_ReceivingHeader,
			State_ReceivingData,
			State_ReceivedPacket,
			State_Send,
			State_WaitingForAck
		};

		typedef uint8 Header[ 3u ];

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
		PortSerialConfig			m_config;

		Serial						Serial1;
#endif

		State						m_state;
		bool						m_connectionReset;
		uintreg						m_lastSendId;
		uint32						m_lastSendTime;
		uintreg						m_counter;
		Header						m_receiveHeader;
		Header						m_sendHeader;
		uint8						m_buffer[ 32u ];

		uint16						getMagicFromHeader( const Header& header ) const;
		uint8						getSizeFromHeader( const Header& header ) const;
		Type						getTypeFromHeader( const Header& header ) const;
		uint8						getIdFromHeader( const Header& header ) const;

		void						writeSendHeader( uint8 size, Type type, uint8 id );

		bool						updateReceiveHeader();
		bool						updateReceiveData();
		bool						updateSend();
		bool						updateWaitForAck();

		void						sendHello();
		void						sendAck( uintreg packetId );

		uint8						calculateChecksum( const Header& header ) const;
	};
}