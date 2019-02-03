#include "conct_port_nrf24l01_server.h"

#include "conct_memory.h"
#include "conct_reader.h"
#include "conct_string_tools.h"
#include "conct_trace.h"
#include "conct_writer.h"

namespace conct
{
	bool PortNRF24L01Server::setup( const PortNRF24L01ServerParameters& parameters )
	{
		if( !m_radio0.begin( parameters.cePin0, 0 ) )
		{
			return false;
		}

		if( parameters.cePin1 != 0 &&
			!m_radio1.begin( parameters.cePin1, 1 ) )
		{
			return false;
		}

		getAddressForPipe( m_protocolAddress, 0u, 0u );
		m_radio0.openReadingPipe( 0u, m_protocolAddress.data );

		const uintreg radioCount = (parameters.cePin1 != 0 ? 2u : 1u);
		const uintreg connectionCount = ((radioCount * PipesPerRadio) - 1u);
		m_connections.setLength( connectionCount );

		for( uintreg i = 0u; i < m_connections.getLength(); ++i )
		{
			const uintreg pipeId = i + 1u;
			Connection& connection = m_connections[ i ];

			connection.radioIndex	= pipeId / PipesPerRadio;
			connection.pipeIndex	= pipeId % PipesPerRadio;

			getAddressForPipe( connection.address, connection.radioIndex, connection.pipeIndex );
		}

		return true;
	}

	bool PortNRF24L01Server::popConnectionReset( uintreg& endpointId )
	{
		if( m_brokenConnections.isEmpty() )
		{
			return false;
		}

		endpointId = m_brokenConnections.getBack();
		m_brokenConnections.popBack();

		return true;
	}

	void PortNRF24L01Server::loop()
	{
		readFromRadio( m_radio0, 0u );
		readFromRadio( m_radio1, 1u );

		const uint32 time = millis();
		for( Connection& connection : m_connections )
		{
			if( !connection.flags.isSet( PortNRF24L01::ConnectionFlag_Connected ) )
			{
				continue;
			}

			if( !connection.flags.isSet( PortNRF24L01::ConnectionFlag_AcknowledgedPacket ) &&
				connection.lastSendId > 0u )
			{
				if( time - connection.lastSendTime > PacketResendTime )
				{
					sendLastDataPacket( connection );
				}
			}
			else if( !connection.sendBuffer.isEmpty() )
			{
				sendDataPacket( connection );
			}
		}
	}

	bool PortNRF24L01Server::openSend( Writer& writer, uintreg size, uintreg endpointId )
	{
		Connection& connection = m_connections[ endpointId ];

		const uintreg sendBufferLength = connection.sendBuffer.getLength();
		connection.sendBuffer.setLength( sendBufferLength + size );

		writer.set( &connection.sendBuffer[ sendBufferLength ], size );
		return true;
	}

	void PortNRF24L01Server::closeSend( Writer& writer, uintreg endpointId )
	{
		Connection& connection = m_connections[ endpointId ];
		connection.sendBuffer.setLength( connection.sendBuffer.getLength() - writer.getRemainingSize() );
	}

	bool PortNRF24L01Server::openReceived( Reader& reader, uintreg& endpointId )
	{
		Connection& connection = m_connections[ endpointId ];
		if( connection.receiveBuffer.isEmpty() )
		{
			return false;
		}

		reader.set( connection.receiveBuffer.getBegin(), connection.receiveBuffer.getLength() );
		return true;
	}

	void PortNRF24L01Server::closeReceived( Reader& reader, uintreg endpointId )
	{
		Connection& connection = m_connections[ endpointId ];

		const uintreg remainingOffset = connection.receiveBuffer.getLength() - reader.getRemainingSize();
		for( uint i = 0u; i < reader.getRemainingSize(); ++i )
		{
			connection.receiveBuffer[ i ] = connection.receiveBuffer[ remainingOffset + i ];
		}
		connection.receiveBuffer.setLength( reader.getRemainingSize() );
	}

	Flags8< PortFlag > PortNRF24L01Server::getFlags() const
	{
		Flags8< PortFlag > flags;
		flags |= PortFlag_MultiEndpoint;
		flags |= PortFlag_Reliable;
		return flags;
	}

	void PortNRF24L01Server::readFromRadio( RF24& radio, uintreg radioIndex )
	{
		if( !radio.isValid() )
		{
			return;
		}

		uint8 pipeIndex;
		while( radio.available( &pipeIndex ) )
		{
			Buffer receiveBuffer;
			radio.read( receiveBuffer, sizeof( receiveBuffer ) );

			if( radioIndex == 0u && pipeIndex == 0u )
			{
				readConnectionRequest( receiveBuffer );
				continue;
			}

			const uintreg endpointId = getEndpointIdForPipe( radioIndex, pipeIndex );
			if( endpointId > m_connections.getLength() )
			{
				trace::write( "Receive data for not connected pipe '"_s + string_tools::toString( pipeIndex ) + "' on radio '" + string_tools::toString( radioIndex ) + "'" );
				continue;
			}

			Connection& connection = m_connections[ endpointId ];
			readPacketForConnection( connection, receiveBuffer );
		}
	}

	bool PortNRF24L01Server::readPacketHeader( uint8& packetSize, uint8& packetId, PacketType& packetType, const Buffer& buffer )
	{
		const uint16 magic	= getMagicFromHeader( buffer );
		packetSize			= getSizeFromHeader( buffer );
		packetId			= getIdFromHeader( buffer );
		packetType			= getTypeFromHeader( buffer );

		if( magic != 0xe670u )
		{
			trace::write( "Receive packet with invalid magic '"_s + string_tools::toHexString( magic ) + "'." );
			return false;
		}
		else if( packetSize > MaxPacketPayloadSize )
		{
			trace::write( "Receive packet with invalid size '"_s + string_tools::toString( packetSize ) + "'." );
			return false;
		}

		const uint8 fullPacketSize = sizeof( Header ) + packetSize;
		const uint8 expectedChecksum = calculateChecksum( buffer, packetSize );
		const uint8 receivedChecksum = buffer[ fullPacketSize ];
		if( receivedChecksum != expectedChecksum )
		{
			trace::write( "Receive packet with invalid checksum '"_s + string_tools::toHexString( receivedChecksum ) + "' but expected '" + string_tools::toHexString( expectedChecksum ) + "'." );
			return false;
		}

		return true;
	}

	void PortNRF24L01Server::readConnectionRequest( const Buffer& buffer )
	{
		uint8 packetSize;
		uint8 packetId;
		PacketType packetType;
		if( !readPacketHeader( packetSize, packetId, packetType, buffer ) )
		{
			return;
		}

		if( packetType != PacketType_Protocol )
		{
			trace::write( "Receive data packet on protocol pipe"_s );
			return;
		}

		handleProtocolMessage( buffer + sizeof( Header ), packetSize, nullptr );
	}

	void PortNRF24L01Server::readPacketForConnection( Connection& connection, const Buffer& buffer )
	{
		uint8 packetSize;
		uint8 packetId;
		PacketType packetType;
		if( !readPacketHeader( packetSize, packetId, packetType, buffer ) )
		{
			trace::write( "could not read packet header from pipe '"_s + string_tools::toString( connection.pipeIndex ) + "' on radio '" + string_tools::toString( connection.radioIndex ) + "'" );
			return;
		}

		if( packetType == PacketType_Protocol )
		{
			handleProtocolMessage( buffer + sizeof( Header ), packetSize, &connection );
		}
		else
		{
			if( packetId == connection.lastReceiveId )
			{
				sendAcknowledgeMessage( connection, packetId );
				return;
			}
			else if( packetId != connection.lastReceiveId + 1u )
			{
				trace::write( "Receive packet with invalid id '"_s + string_tools::toString( packetId ) + "' but expected '" + string_tools::toString( connection.lastReceiveId + 1u ) + "' from pipe '"_s + string_tools::toString( connection.pipeIndex ) + "' on radio '" + string_tools::toString( connection.radioIndex ) + "'" );
				resetConnection( connection );
				return;
			}

			connection.receiveBuffer.pushRange( buffer + sizeof( Header ), packetSize );

			connection.lastReceiveId = packetId;
			sendAcknowledgeMessage( connection, packetId );
		}
	}

	void PortNRF24L01Server::handleProtocolMessage( const uint8* pData, uintreg size, Connection* pConnection )
	{
		const ProtocolMessageHeader* pMessageHeader = (const ProtocolMessageHeader*)pData;
		switch( pMessageHeader->messageType )
		{
		case ProtocolMessageType_Request:
			{
				if( pConnection != nullptr )
				{
					trace::write( "Received connection request from a already connected pipe '"_s + string_tools::toString( pConnection->pipeIndex ) + "' on radio '" + string_tools::toString( pConnection->radioIndex ) + "'" );
					return;
				}

				const RequestProtocolMessageData* pRequestData = (const RequestProtocolMessageData*)&pMessageHeader[ 1u ];
				for( uint i = 0u; i < m_connections.getLength(); ++i )
				{
					Connection& connection = m_connections[ i ];
					if( connection.flags.isSet( ConnectionFlag_Connected ) )
					{
						continue;
					}

					connection.flags.clear();
					connection.flags.set( ConnectionFlag_Connected );
					connection.flags.set( ConnectionFlag_AcknowledgedPacket );

					connection.lastSendTime		= 0u;
					connection.lastSendId		= 0u;
					connection.lastReceiveId	= 0u;

					sendAddressMessage( pRequestData->requestId, connection.address );
				}

				sendDepletedMessage( pRequestData->requestId );
			}
			break;

		case ProtocolMessageType_Address:
			{
				trace::write( "Received connection address on server." );
				return;
			}
			break;

		case ProtocolMessageType_Depleted:
			{
				trace::write( "Received depleted on server." );
				return;
			}
			break;

		case ProtocolMessageType_Ack:
			{
				if( pConnection == nullptr )
				{
					trace::write( "Received acknowledge on protocol pipe." );
					return;
				}

				const AcknowledgeProtocolMessageData* pAckData = (const AcknowledgeProtocolMessageData*)&pMessageHeader[ 1u ];
				if( pConnection->lastSendId != pAckData->packetId )
				{
					trace::write( "Received acknowledge for an wrong packet. Received id '"_s + string_tools::toString( pAckData->packetId ) + "' doesn't match last send '" + string_tools::toString( pConnection->lastSendId ) + "'." );
					resetConnection( *pConnection );
					return;
				}

				pConnection->flags.set( ConnectionFlag_AcknowledgedPacket );
			}
			break;

		case ProtocolMessageType_Reset:
			{
				trace::write( "Received reset on server." );
				return;
			}
			break;
		}
	}

	void PortNRF24L01Server::resetConnection( Connection& connection )
	{
		if( connection.flags.isSet( ConnectionFlag_Connected ) )
		{
			Buffer buffer;
			uint8 payloadSize;
			writeProtocolHeader( buffer, payloadSize, ProtocolMessageType_Reset, 0u );
			const uint8 packetSize = finalizePacket( buffer, payloadSize );

			RF24& radio = connection.radioIndex == 0u ? m_radio0 : m_radio1;
			radio.openWritingPipe( connection.address.data );
			radio.stopListening();
			radio.write( buffer, packetSize );
			radio.startListening();
		}

		connection.lastSendTime		= 0u;
		connection.lastSendId		= 0u;
		connection.lastReceiveId	= 0u;

		connection.flags.clear();
		connection.sendBuffer.clear();
		connection.receiveBuffer.clear();

		m_brokenConnections.pushBack( &connection - m_connections.getBegin() );
	}

	void PortNRF24L01Server::sendAddressMessage( uint32 requestId, const Address& address )
	{
		Buffer buffer;

		uint8 payloadSize;
		AddressProtocolMessageData* pAddressData = (AddressProtocolMessageData*)writeProtocolHeader( buffer, payloadSize, ProtocolMessageType_Address, sizeof( AddressProtocolMessageData ) );
		pAddressData->requestId	= requestId;
		pAddressData->address	= address;
		const uint8 packetSize = finalizePacket( buffer, payloadSize );

		m_radio0.openWritingPipe( m_protocolAddress.data );
		m_radio0.stopListening();
		m_radio0.write( buffer, packetSize );
		m_radio0.startListening();
	}

	void PortNRF24L01Server::sendDepletedMessage( uint32 requestId )
	{
		Buffer buffer;

		uint8 payloadSize;
		DepletedProtocolMessageData* pDepletedData = (DepletedProtocolMessageData*)writeProtocolHeader( buffer, payloadSize, ProtocolMessageType_Depleted, sizeof( DepletedProtocolMessageData ) );
		pDepletedData->requestId = requestId;
		const uint8 packetSize = finalizePacket( buffer, payloadSize );

		m_radio0.openWritingPipe( m_protocolAddress.data );
		m_radio0.stopListening();
		m_radio0.write( buffer, packetSize );
		m_radio0.startListening();
	}

	void PortNRF24L01Server::sendAcknowledgeMessage( Connection& connection, uint8 packetId )
	{
		Buffer buffer;
		uint8 payloadSize;
		AcknowledgeProtocolMessageData* pAcknowledgeData = (AcknowledgeProtocolMessageData*)writeProtocolHeader( buffer, payloadSize, ProtocolMessageType_Ack, sizeof( AcknowledgeProtocolMessageData ) );
		pAcknowledgeData->packetId = packetId;
		const uint8 packetSize = finalizePacket( buffer, payloadSize );

		RF24& radio = connection.radioIndex == 0u ? m_radio0 : m_radio1;
		radio.openWritingPipe( connection.address.data );
		radio.stopListening();
		radio.write( buffer, packetSize );
		radio.startListening();
	}

	void PortNRF24L01Server::sendDataPacket( Connection& connection )
	{
		const uint8 payloadSize = CONCT_MIN( connection.sendBuffer.getLength(), MaxPacketPayloadSize );

		do
		{
			connection.lastSendId = (connection.lastSendId + 1u) & 0x3f;
		}
		while( connection.lastSendId == 0u );

		uint8* pData = writeHeader( connection.lastSendPacket, payloadSize, PacketType_Data, connection.lastSendId );
		memory::copy( pData, connection.sendBuffer.getBegin(), payloadSize );
		connection.lastSendPacketSize = finalizePacket( connection.lastSendPacket, payloadSize );

		const uintreg remainingSize = connection.sendBuffer.getLength() - payloadSize;
		const uintreg remainingOffset = connection.sendBuffer.getLength() - remainingSize;
		for( uint i = 0u; i < remainingSize; ++i )
		{
			connection.sendBuffer[ i ] = connection.sendBuffer[ remainingOffset + i ];
		}
		connection.sendBuffer.setLength( remainingSize );

		sendLastDataPacket( connection );
	}

	void PortNRF24L01Server::sendLastDataPacket( Connection& connection )
	{
		RF24& radio = connection.radioIndex == 0u ? m_radio0 : m_radio1;
		radio.openWritingPipe( connection.address.data );
		radio.stopListening();
		radio.write( connection.lastSendPacket, connection.lastSendPacketSize );
		radio.startListening();

		connection.lastSendTime = millis();
	}

	uintreg PortNRF24L01Server::getEndpointIdForPipe( uintreg radioIndex, uintreg pipeIndex ) const
	{
		return ((radioIndex * PipesPerRadio) + pipeIndex) - 1u;
	}
}
