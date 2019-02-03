#pragma once

#include "conct_port_nrf24l01.h"

#include <RF24.h>

namespace conct
{
	struct PortNRF24L01ClientParameters
	{
		int							cePin;
		int							csPin;
	};

	class PortNRF24L01Client : public PortNRF24L01
	{
	public:

		bool						setup( const PortNRF24L01ClientParameters& parameters );

		virtual bool				popConnectionReset( uintreg& endpointId ) CONCT_OVERRIDE_FINAL;

		virtual void				loop() CONCT_OVERRIDE_FINAL;

		virtual bool				openSend( Writer& writer, uintreg size, uintreg endpointId ) CONCT_OVERRIDE_FINAL;
		virtual void				closeSend( Writer& writer, uintreg endpointId ) CONCT_OVERRIDE_FINAL;

		virtual bool				openReceived( Reader& reader, uintreg& endpointId ) CONCT_OVERRIDE_FINAL;
		virtual void				closeReceived( Reader& reader, uintreg endpointId ) CONCT_OVERRIDE_FINAL;

		virtual Flags8< PortFlag >	getFlags() const CONCT_OVERRIDE_FINAL;

	private:

		enum ConnectionFlag : uint8
		{
			ConnectionFlag_Connected			= 1u << 0u,
			ConnectionFlag_ConnectionReset		= 1u << 1u,
			ConnectionFlag_ReceivedPacket		= 1u << 2u,
			ConnectionFlag_AcknowledgedPacket	= 1u << 3u
		};

		RF24						m_radio;
		Address						m_address;

		Flags8< ConnectionFlag >	m_flags;

		uint32						m_requestId;
		uint32						m_lastSendTime;
		uint8						m_lastSendId;
		uint8						m_lastSendSize;
		uint8						m_lastReceiveId;
		uint8						m_lastReceiveSize;

		Buffer						m_receiveBuffer;
		Buffer						m_sendBuffer;

		void						updateConnection();
		void						resetConnection();

		void						sendPacket();
		void						receivePacket();

		void						handleProtocolMessage();
	};
}