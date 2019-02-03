#include "conct_port_nrf24l01_server.h"

#include "conct_reader.h"
#include "conct_string_tools.h"
#include "conct_trace.h"
#include "conct_writer.h"

namespace conct
{
	static const uint32 s_resendTime = 100;

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

		for( Connection& connection : m_connections )
		{
			if( !connection.flags.isSet( PortNRF24L01::ConnectionFlag_Connected ) ||
				!connection.flags.isSet( PortNRF24L01::ConnectionFlag_AcknowledgedPacket ) ||
				connection.receiveBuffer.isEmpty() )
			{
				continue;
			}

			sendDataPacket( connection );
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
		if( reader.isEnd() )
		{
			connection.receiveBuffer.clear();
			return;
		}

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
			trace::write( "Receive packet with invalid magic '"_s + string_tools::toHexString( magic ) + "' from pipe '"_s + string_tools::toString( connection.pipeIndex ) + "' on radio '" + string_tools::toString( connection.radioIndex ) + "'" );
			return false;
		}
		else if( packetSize > 28 )
		{
			trace::write( "Receive packet with invalid size '"_s + string_tools::toString( packetSize ) + "' from pipe '"_s + string_tools::toString( connection.pipeIndex ) + "' on radio '" + string_tools::toString( connection.radioIndex ) + "'" );
			return false;
		}

		const uint8 fullPacketSize = sizeof( Header ) + packetSize;
		const uint8 expectedChecksum = calculateChecksum( buffer, sizeof( Header ) + packetSize );
		const uint8 receivedChecksum = buffer[ fullPacketSize ];
		if( receivedChecksum != expectedChecksum )
		{
			trace::write( "Receive packet with invalid checksum '"_s + string_tools::toHexString( receivedChecksum ) + "' but expected '" + string_tools::toHexString( expectedChecksum ) + "' from pipe '"_s + string_tools::toString( connection.pipeIndex ) + "' on radio '" + string_tools::toString( connection.radioIndex ) + "'" );
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
			uint8 packetSize;
			writeProtocolHeader( buffer, packetSize, ProtocolMessageType_Reset, 0u );

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

		uint8 packetSize;
		AddressProtocolMessageData* pAddressData = (AddressProtocolMessageData*)writeProtocolHeader( buffer, packetSize, ProtocolMessageType_Address, sizeof( AddressProtocolMessageData ) );
		pAddressData->requestId	= requestId;
		pAddressData->address	= address;

		m_radio0.openWritingPipe( m_protocolAddress.data );
		m_radio0.stopListening();
		m_radio0.write( buffer, packetSize );
		m_radio0.startListening();
	}

	void PortNRF24L01Server::sendDepletedMessage( uint32 requestId )
	{
		Buffer buffer;

		uint8 packetSize;
		DepletedProtocolMessageData* pDepletedData = (DepletedProtocolMessageData*)writeProtocolHeader( buffer, packetSize, ProtocolMessageType_Depleted, sizeof( DepletedProtocolMessageData ) );
		pDepletedData->requestId = requestId;

		m_radio0.openWritingPipe( m_protocolAddress.data );
		m_radio0.stopListening();
		m_radio0.write( buffer, packetSize );
		m_radio0.startListening();
	}

	void PortNRF24L01Server::sendAcknowledgeMessage( Connection& connection, uint8 packetId )
	{
		Buffer buffer;
		uint8 packetSize;
		AcknowledgeProtocolMessageData* pAcknowledgeData = (AcknowledgeProtocolMessageData*)writeProtocolHeader( buffer, packetSize, ProtocolMessageType_Ack, sizeof( AcknowledgeProtocolMessageData ) );
		pAcknowledgeData->packetId = packetId;

		RF24& radio = connection.radioIndex == 0u ? m_radio0 : m_radio1;
		radio.openWritingPipe( connection.address.data );
		radio.stopListening();
		radio.write( buffer, packetSize );
		radio.startListening();
	}

	void PortNRF24L01Server::sendDataPacket( Connection& connection )
	{

	}

	uintreg PortNRF24L01Server::getEndpointIdForPipe( uintreg radioIndex, uintreg pipeIndex ) const
	{
		return ((radioIndex * PipesPerRadio) + pipeIndex) - 1u;
	}

//	bool PortNRF24L01Server::popConnectionReset( uintreg& endpointId )
//	{
//		if( !m_flags.isSet( Flag_ConnectionReset ) )
//		{
//			return false;
//		}
//
//		endpointId = 0u;
//
//		m_counter	= 0u;
//		m_state		= State_Idle;
//		m_flags.unset( Flag_ConnectionReset );
//
//		return true;
//	}
//
//	bool PortNRF24L01Server::setup( const PortNRF24L01ServerParameters& parameters )
//	{
//		m_state					= State_Idle;
//		m_lastSendId			= 0u;
//		m_lastLastReceivedId	= 0u;
//		m_counter				= 0u;
//		m_flags.clear();
//
//		m_radio.begin( parameters.cePin, 0 );
//
//		sendHello();
//
//		return true;
//	}
//
//	void PortNRF24L01Server::loop()
//	{
//		bool running = true;
//		while( running )
//		{
//			switch( m_state )
//			{
//			case State_Idle:
//				if( m_flags.isSet( Flag_ReceivedPacket ) )
//				{
//					running = false;
//				}
//				else
//				{
//					running = updateReceiveHeader();
//				}
//				break;
//
//			case State_ReceivingData:
//				running = updateReceiveData();
//				break;
//
//			case State_Send:
//				running = updateSend();
//				break;
//
//			case State_WaitingForAck:
//				running = updateWaitForAck();
//				break;
//			}
//		}
//	}
//
//	bool PortNRF24L01Server::openSend( Writer& writer, uintreg size, uintreg endpointId )
//	{
//		if( m_state != State_Idle )
//		{
//			return false;
//		}
//
//		writer.set( m_buffer, sizeof( m_buffer ) - 1u );
//		return true;
//	}
//
//	void PortNRF24L01Server::closeSend( Writer& writer, uintreg endpointId )
//	{
//		m_counter = 0u;
//		m_lastSendId = (m_lastSendId + 1u) % 32u;
//
//		const uint8 size = uint8( sizeof( m_buffer ) - writer.getRemainingSize() - 1u );
//		writeSendHeader( size, Type_Data, m_lastSendId );
//		m_buffer[ size ] = calculateChecksum( m_sendHeader );
//
//		m_state = State_Send;
//	}
//
//	bool PortNRF24L01Server::openReceived( Reader& reader, uintreg& endpointId )
//	{
//		if( !m_flags.isSet( Flag_ReceivedPacket ) )
//		{
//			return false;
//		}
//
//		endpointId = 0u;
//		reader.set( m_buffer, getSizeFromHeader( m_receiveHeader ) );
//		return true;
//	}
//
//	void PortNRF24L01Server::closeReceived( Reader& reader, uintreg endpointId )
//	{
//		m_counter = 0u;
//		m_state = State_Idle;
//		m_flags.unset( Flag_ReceivedPacket );
//	}
//
//	Flags8< PortFlag > PortNRF24L01Server::getFlags()
//	{
//		Flags8< PortFlag > flags;
//		flags |= PortFlag_SingleEndpoint;
//		flags |= PortFlag_Reliable;
//		return flags;
//	}
//

//
//	bool PortNRF24L01Server::updateReceiveHeader()
//	{
//		while( Serial1.available() )
//		{
//			const uint8 byte = Serial1.read();
//			m_receiveHeader[ m_counter++ ] = byte;
//
//			static const char s_aHexChars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
//#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
//			char hexBuffer[] = { s_aHexChars[ ( byte & 0xf0u ) >> 4u ], s_aHexChars[ byte & 0x0fu ], 0u };
//			char charBuffer[] ={ ' ', byte, 0u };
//			m_hexTrace += hexBuffer;
//			m_charTrace += charBuffer;
//#elif CONCT_ENABLED( CONCT_PLATFORM_ARDUINO )
//			Serial.write( s_aHexChars[ ( byte & 0xf0u ) >> 4u ] );
//			Serial.write( s_aHexChars[ byte & 0x0fu ] );
//#endif
//
//			if( m_counter == 2u )
//			{
//				if( getMagicFromHeader( m_receiveHeader ) != 0x42b0u )
//				{
//					m_receiveHeader[ 0u ] = m_receiveHeader[ 1u ];
//					m_counter = 1u;
//				}
//			}
//			else if( m_counter == 3u )
//			{
//#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
//				trace::write( m_hexTrace );
//				trace::write( m_charTrace );
//				m_hexTrace.clear();
//				m_charTrace.clear();
//#elif CONCT_ENABLED( CONCT_PLATFORM_ARDUINO )
//				Serial.write( '\n' );
//#endif
//
//				const Type packetType = getTypeFromHeader( m_receiveHeader );
//				if( packetType == Type_Hello )
//				{
//					m_flags.set( Flag_ConnectionReset );
//					sendAck( getIdFromHeader( m_receiveHeader ) );
//					return true;
//				}
//				else if( packetType == Type_Data )
//				{
//					if( m_state == State_WaitingForAck )
//					{
//						m_counter = 0u;
//						continue;
//					}
//
//					m_counter = 0u;
//					m_state = State_ReceivingData;
//					return true;
//				}
//				else if( packetType == Type_Ack )
//				{
//					if( getIdFromHeader( m_receiveHeader ) == m_lastSendId )
//					{
//						m_counter = 0u;
//						m_state = State_Idle;
//					}
//					else
//					{
//						m_flags.set( Flag_ConnectionReset );
//						sendHello();
//					}
//					return true;
//				}
//			}
//			CONCT_ASSERT( m_counter < 3u );
//		}
//
//		return false;
//	}
//
//	bool PortNRF24L01Server::updateReceiveData()
//	{
//		const uint8 dataSize = getSizeFromHeader( m_receiveHeader );
//		while( Serial1.available() )
//		{
//			if( m_counter < dataSize )
//			{
//				m_buffer[ m_counter++ ] = Serial1.read();
//			}
//			else
//			{
//				const uint8 receivedChecksum = Serial1.read();
//				const uint8 localChecksum = calculateChecksum( m_receiveHeader );
//
//				const uint8 id = getIdFromHeader( m_receiveHeader );
//				if( receivedChecksum == localChecksum )
//				{
//					if( id != m_lastLastReceivedId )
//					{
//						m_flags.set( Flag_ReceivedPacket );
//					}
//					m_lastLastReceivedId = id;
//					sendAck( getIdFromHeader( m_receiveHeader ) );
//					return true;
//				}
//				else
//				{
//					m_counter = 0u;
//					m_state = State_Idle;
//					return true;
//				}
//			}
//		}
//
//		return false;
//	}
//
//	bool PortNRF24L01Server::updateSend()
//	{
//		for( uintreg i = 0u; i < sizeof( m_sendHeader ); ++i )
//		{
//			Serial1.write( m_sendHeader[ i ] );
//		}
//
//		const uint8 dataSize = getSizeFromHeader( m_sendHeader );
//		for( uintreg i = 0u; i <= dataSize; ++i )
//		{
//			Serial1.write( m_buffer[ i ] );
//		}
//
//#if CONCT_ENABLED( CONCT_PLATFORM_ARDUINO )
//		while( !( UCSR1A & ( 1 << UDRE1 ) ) )  // Wait for empty transmit buffer
//		{
//			UCSR1A |= 1 << TXC1;  // mark transmission not complete
//		}
//		while( !( UCSR1A & ( 1 << TXC1 ) ) );   // Wait for the transmission to complete
//#endif
//
//		m_lastSendTime = millis();
//
//		if( getTypeFromHeader( m_sendHeader ) != Type_Ack )
//		{
//			m_counter = 0u;
//			m_state = State_WaitingForAck;
//			return false;
//		}
//
//		m_counter = 0u;
//		m_state = State_Idle;
//		return false;
//	}
//
//	bool PortNRF24L01Server::updateWaitForAck()
//	{
//		const uint32 currentTime = millis();
//		if( currentTime < m_lastSendTime || m_lastSendTime + s_resendTime < currentTime ) // first condition is for overflow
//		{
//			m_counter = 0u;
//			m_state = State_Send;
//			return true;
//		}
//
//		if( updateReceiveHeader() )
//		{
//			return true;
//		}
//
//		return false;
//	}
//
//	void PortNRF24L01Server::sendHello()
//	{
//		writeSendHeader( 0u, Type_Hello, 0u );
//
//		m_lastSendId	= 0u;
//		m_counter		= 0u;
//		m_state			= State_Send;
//	}
//
//	void PortNRF24L01Server::sendAck( uint8 packetId )
//	{
//		writeSendHeader( 0u, Type_Ack, packetId );
//
//		m_counter		= 0u;
//		m_state			= State_Send;
//	}
//
//	uint8 PortNRF24L01Server::calculateChecksum( const Header& header ) const
//	{
//		// source: https://stackoverflow.com/questions/13491700/8-bit-fletcher-checksum-of-16-byte-data
//		uint32 sum1 = 1;
//		uint32 sum2 = 0;
//
//		const uint8* pData = header;
//		const uint8* pDataEnd = pData + sizeof( header );
//		while( pData < pDataEnd )
//		{
//			sum2 += sum1 += *pData++;
//		}
//
//		pData = m_buffer;
//		pDataEnd = pData + getSizeFromHeader( header );
//		while( pData < pDataEnd )
//		{
//			sum2 += sum1 += *pData++;
//		}
//
//		sum1 %= 15u;
//		sum2 %= 15u;
//
//		// bit shift modulo 15
//		//k = ( k >> 16 ) + ( k & 0xffff );
//		//k = ( k >> 8 ) + ( k & 0xff );
//		//k = ( k >> 4 ) + ( k & 0xf );
//		//k = ( k >> 4 ) + ( k & 0xf );
//		//if( k > 14 )
//		//{
//		//	k -= 15;
//		//}
//
//		return ( sum2 << 4u ) + sum1;
//	}
}
