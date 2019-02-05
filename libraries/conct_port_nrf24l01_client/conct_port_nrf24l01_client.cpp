#include "conct_port_nrf24l01_client.h"

#include "conct_reader.h"
#include "conct_writer.h"

namespace conct
{
	bool PortNRF24L01Client::setup( const PortNRF24L01ClientParameters& parameters )
	{
		if( !m_radio.begin( parameters.cePin, parameters.csPin ) )
		{
			return false;
		}

		m_radio.setPALevel( RF24_PA_MIN );

		resetConnection();
		m_flags.clear();

		return true;
	}

	void PortNRF24L01Client::getEndpoints( ArrayView< uintreg >& endpoints )
	{
		if( m_flags.isSet( ConnectionFlag_Connected ) )
		{
			static const uintreg s_endpointId = 0u;
			endpoints.set( &s_endpointId, 1u );
		}
		else
		{
			endpoints.set( nullptr, 0u );
		}
	}

	bool PortNRF24L01Client::popConnectionReset( uintreg& endpointId )
	{
		if( m_flags.isSet( ConnectionFlag_ConnectionReset ) )
		{
			m_flags.unset( ConnectionFlag_ConnectionReset );
			return true;
		}

		return false;
	}

	void PortNRF24L01Client::loop()
	{
		if( !m_flags.isSet( ConnectionFlag_Connected ) )
		{
			updateConnection();
		}

		if( !m_flags.isSet( ConnectionFlag_ReceivedPacket ) )
		{
			receivePacket();
		}

		if( m_lastSendId != 0u &&
			!m_flags.isSet( ConnectionFlag_AcknowledgedPacket ) )
		{
			const uint32 elapsedTime = (millis() - m_lastSendTime);
			if( elapsedTime > PacketResendTime )
			{
				sendPacket();
			}
		}
	}

	bool PortNRF24L01Client::openSend( Writer& writer, uintreg size, uintreg endpointId )
	{
		if( !m_flags.isSet( ConnectionFlag_Connected ) ||
			(m_lastSendId != 0u && !m_flags.isSet( ConnectionFlag_AcknowledgedPacket )) )
		{
			return false;
		}

		size = CONCT_MIN( size, MaxPacketPayloadSize );
		writer.set( m_sendBuffer + sizeof( Header ), size );
		m_lastSendSize = size;
		return true;
	}

	void PortNRF24L01Client::closeSend( Writer& writer, uintreg endpointId )
	{
		m_lastSendSize -= writer.getRemainingSize();

		do
		{
			m_lastSendId = (m_lastSendId + 1u) & PacketIdMask;
		}
		while( m_lastSendId == 0u );

		m_flags.unset( ConnectionFlag_AcknowledgedPacket );

		writeHeader( m_sendBuffer, m_lastSendSize, PacketType_Data, m_lastSendId );
		finalizePacket( m_sendBuffer, m_lastSendSize );
		sendPacket();
	}

	bool PortNRF24L01Client::openReceived( Reader& reader, uintreg& endpointId )
	{
		if( !m_flags.isSet( ConnectionFlag_ReceivedPacket ) )
		{
			return false;
		}

		endpointId = 0u;
		reader.set( m_receiveBuffer + sizeof( Header ), m_lastReceiveSize );
		return true;
	}

	void PortNRF24L01Client::closeReceived( Reader& reader, uintreg endpointId )
	{
		m_flags.unset( ConnectionFlag_ReceivedPacket );
	}

	Flags8< PortFlag > PortNRF24L01Client::getFlags() const
	{
		Flags8< PortFlag > flags;
		flags |= PortFlag_SingleEndpoint;
		flags |= PortFlag_Reliable;
		return flags;
	}

	void PortNRF24L01Client::updateConnection()
	{
		const uint32 elapsedTime = (millis() - m_lastSendTime);
		if( elapsedTime < MaxPacketPayloadSize )
		{
			return;
		}

		RequestProtocolMessageData* pRequestData = (RequestProtocolMessageData*)writeProtocolHeader( m_sendBuffer, m_lastSendSize, PortNRF24L01::ProtocolMessageType_Request, sizeof( RequestProtocolMessageData ) );
		pRequestData->requestId = m_requestId;
		finalizePacket( m_sendBuffer, m_lastSendSize );

		sendPacket();
	}

	void PortNRF24L01Client::resetConnection()
	{
		Address protocolServerAddress;
		Address protocolClientAddress;
		getAddressForPipe( protocolServerAddress, 0u, 0u, true );
		getAddressForPipe( protocolClientAddress, 0u, 0u, false );

		m_radio.stopListening();
		m_radio.openWritingPipe( protocolServerAddress.data );
		m_radio.openReadingPipe( 1u, protocolClientAddress.data );
		m_radio.startListening();

		m_lastSendTime		= 0u;
		m_lastSendId		= 0u;
		m_lastSendSize		= 0u;
		m_lastReceiveId		= 0u;
		m_lastReceiveSize	= 0u;

		m_flags = ConnectionFlag_ConnectionReset;
	}

	void PortNRF24L01Client::sendPacket()
	{
		m_radio.stopListening();
		m_radio.write( m_sendBuffer, sizeof( Header ) + m_lastSendSize + 1u );
		m_radio.startListening();

		m_lastSendTime = millis();
	}

	void PortNRF24L01Client::receivePacket()
	{
		if( !m_radio.available() )
		{
			return;
		}

		m_radio.read( m_receiveBuffer, sizeof( m_receiveBuffer ) );

		const uint16 magic = getMagicFromHeader( m_receiveBuffer );
		if( magic != PacketMagic )
		{
			//trace::write( "Receive packet with invalid magic '"_s + string_tools::toHexString( magic ) + "'." );
			return;
		}

		const uint8 packetSize = getSizeFromHeader( m_receiveBuffer );
		if( packetSize > MaxPacketPayloadSize )
		{
			//trace::write( "Receive packet with invalid size '"_s + string_tools::toString( packetSize ) + "'." );
			return;
		}

		const uint8 packetId = getIdFromHeader( m_receiveBuffer );
		const PacketType packetType = getTypeFromHeader( m_receiveBuffer );
		if( packetType != PacketType_Protocol )
		{
			if( packetId == m_lastReceiveId )
			{
				sendAcknowledgeMessage();
				return;
			}

			uint8 expectedPacketId = m_lastReceiveId;
			do
			{
				expectedPacketId = (expectedPacketId + 1u) & PacketIdMask;
			}
			while( expectedPacketId == 0u );

			if( packetId != expectedPacketId )
			{
				return;
			}

			m_lastReceiveId = packetId;
		}

		const uint8 fullPacketSize = sizeof( Header ) + packetSize;
		const uint8 expectedChecksum = calculateChecksum( m_receiveBuffer, packetSize );
		const uint8 receivedChecksum = m_receiveBuffer[ fullPacketSize ];
		if( receivedChecksum != expectedChecksum )
		{
			//trace::write( "Receive packet with invalid checksum '"_s + string_tools::toHexString( receivedChecksum ) + "' but expected '" + string_tools::toHexString( expectedChecksum ) + "'." );
			return;
		}

		m_lastReceiveSize = packetSize;

		if( packetType == PacketType_Protocol )
		{
			handleProtocolMessage();
		}
		else if( m_flags.isSet( ConnectionFlag_Connected ) )
		{
			sendAcknowledgeMessage();
			m_flags.set( ConnectionFlag_ReceivedPacket );
		}
	}

	void PortNRF24L01Client::handleProtocolMessage()
	{
		const ProtocolMessageHeader* pProtocolHeader = (const ProtocolMessageHeader*)&m_receiveBuffer[ sizeof( Header ) ];
		switch( pProtocolHeader->messageType )
		{
		case ProtocolMessageType_Request:
			break;

		case ProtocolMessageType_Address:
			{
				const AddressProtocolMessageData* pAddressData = (const AddressProtocolMessageData*)&pProtocolHeader[ 1u ];
				if( pAddressData->requestId != m_requestId )
				{
					return;
				}

				Address connectionServerAddress;
				Address connectionClientAddress;
				getAddressForPipe( connectionServerAddress, pAddressData->radioIndex, pAddressData->pipeIndex, true );
				getAddressForPipe( connectionClientAddress, pAddressData->radioIndex, pAddressData->pipeIndex, false );

				m_radio.stopListening();
				m_radio.openWritingPipe( connectionServerAddress.data );
				m_radio.openReadingPipe( 1u, connectionClientAddress.data );
				m_radio.startListening();

				m_flags.set( ConnectionFlag_Connected );
			}
			break;

		case ProtocolMessageType_Depleted:
			{
				const DepletedProtocolMessageData* pDepletedData = (const DepletedProtocolMessageData*)&pProtocolHeader[ 1u ];
				if( pDepletedData->requestId != m_requestId )
				{
					return;
				}

				resetConnection();
			}
			break;

		case ProtocolMessageType_Ack:
			{
				const AcknowledgeProtocolMessageData* pAcknowledgeData = (const AcknowledgeProtocolMessageData*)&pProtocolHeader[ 1u ];
				if( pAcknowledgeData->packetId != m_lastSendId )
				{
					return;
				}

				m_flags.set( ConnectionFlag_AcknowledgedPacket );
			}
			break;

		case ProtocolMessageType_Reset:
			{
				resetConnection();
			}
			break;
		}
	}

	void PortNRF24L01Client::sendAcknowledgeMessage()
	{
		Buffer ackBuffer;
		uint8 payloadSize;
		AcknowledgeProtocolMessageData* pAcknowledgeData = (AcknowledgeProtocolMessageData*)writeProtocolHeader( ackBuffer, payloadSize, ProtocolMessageType_Ack, sizeof( AcknowledgeProtocolMessageData ) );
		pAcknowledgeData->packetId = m_lastReceiveId;
		const uint8 packetSize = finalizePacket( ackBuffer, payloadSize );

		m_radio.stopListening();
		m_radio.write( ackBuffer, packetSize );
		m_radio.startListening();
	}
}
