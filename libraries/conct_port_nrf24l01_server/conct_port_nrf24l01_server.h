#pragma once

#include "conct_port_nrf24l01.h"

#include "conct_vector.h"

#include <RF24.h>

namespace conct
{
	struct PortNRF24L01ServerParameters
	{
#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
		int								cePin0;
		int								cePin1;
#endif
	};

	class PortNRF24L01Server : public PortNRF24L01
	{
	public:

		bool							setup( const PortNRF24L01ServerParameters& parameters );

		virtual void					getEndpoints( ArrayView< uintreg >& endpoints ) CONCT_OVERRIDE_FINAL;
		virtual bool					popConnectionReset( uintreg& endpointId ) CONCT_OVERRIDE_FINAL;

		virtual void					loop() CONCT_OVERRIDE_FINAL;

		virtual bool					openSend( Writer& writer, uintreg size, uintreg endpointId ) CONCT_OVERRIDE_FINAL;
		virtual void					closeSend( Writer& writer, uintreg endpointId ) CONCT_OVERRIDE_FINAL;

		virtual bool					openReceived( Reader& reader, uintreg& endpointId ) CONCT_OVERRIDE_FINAL;
		virtual void					closeReceived( Reader& reader, uintreg endpointId ) CONCT_OVERRIDE_FINAL;

		virtual Flags8< PortFlag >		getFlags() const CONCT_OVERRIDE_FINAL;

	private:

		enum ConnectionFlag : uint8
		{
			ConnectionFlag_Connected			= 1u << 0u,
			ConnectionFlag_AcknowledgedPacket	= 1u << 1u
		};

		struct Connection
		{
			uintreg						radioIndex;
			uintreg						pipeIndex;
			uint32						requestId;
			Address						clientAddress;

			Flags8< ConnectionFlag >	flags;

			uint32						lastSendTime;
			uint8						lastSendId;
			Buffer						lastSendPacket;
			uint8						lastSendPacketSize;

			uint8						lastReceiveId;

			Vector< uint8 >				sendBuffer;
			Vector< uint8 >				receiveBuffer;
		};

		RF24							m_radio0;
		RF24							m_radio1;

		Address							m_protocolServerAddress;
		Address							m_protocolClientAddress;

		Vector< Connection >			m_connections;
		Vector< uintreg >				m_endpoints;;
		Vector< uintreg >				m_brokenConnections;

		void							readFromRadio( RF24& radio, uintreg radioIndex );
		bool							readPacketHeader( uint8& packetSize, uint8& packetId, PacketType& packetType, const Buffer& buffer );
		void							readConnectionRequest( const Buffer& buffer );
		void							readPacketForConnection( Connection& connection, const Buffer& buffer );
		void							handleProtocolMessage( const uint8* pData, uintreg size, Connection* pConnection );

		void							resetConnection( Connection& connection );

		void							sendAddressMessage( uint32 requestId, const Connection& connection );
		void							sendDepletedMessage( uint32 requestId );
		void							sendAcknowledgeMessage( Connection& connection, uint8 packetId );
		void							sendDataPacket( Connection& connection );
		void							sendLastDataPacket( Connection& connection );

		uintreg							getEndpointIdForPipe( uintreg radioIndex, uintreg pipeIndex ) const;
	};
}