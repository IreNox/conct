#include "conct_port_serial.h"

#include "conct_reader.h"
#include "conct_writer.h"

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
#	include "conct_dynamic_string.h"
#	include "conct_memory.h"
#	include "conct_trace.h"

#	include "linux/conct_port_serial_arduino_interface_linux.h"
#elif CONCT_ENABLED( CONCT_ENVIRONMENT_SIMULATOR )
#	include "simulator/conct_port_serial_arduino_interface_sim.h"
#elif CONCT_ENABLED( CONCT_ENVIRONMENT_TESTS )
#	include "test/conct_port_serial_arduino_interface_test.h"
#elif CONCT_ENABLED( CONCT_PLATFORM_ARDUINO )
#	include <arduino.h>
#else
#	error "Unsupported platform"
#endif

namespace conct
{
	static const uint32 s_resendTime = 100;

	bool PortSerial::popConnectionReset( uintreg& endpointId )
	{
		if( !m_flags.isSet( Flag_ConnectionReset ) )
		{
			return false;
		}

		endpointId = 0u;

		m_counter	= 0u;
		m_state		= State_Idle;
		m_flags.unset( Flag_ConnectionReset );

		return true;
	}

	bool PortSerial::setup( const PortSerialParameters& parameters )
	{
		m_state					= State_Idle;
		m_lastSendId			= 0u;
		m_lastLastReceivedId	= 0u;
		m_counter				= 0u;
		m_flags.clear();

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
		if( !Serial1.setup( parameters.portName ) )
		{
			return false;
		}
#elif CONCT_ENABLED( CONCT_PLATFORM_ARDUINO )
		Serial.begin( 9600 );
		Serial.write( "Start!\n" );
#endif

		Serial1.begin( parameters.speed );

		sendHello();

		return true;
	}

	void PortSerial::getEndpoints( ArrayView< uintreg >& endpoints )
	{
		static const uintreg s_endPoint = 0u;
		endpoints.set( &s_endPoint, 1u );
	}

	void PortSerial::loop()
	{
		bool running = true;
		while( running )
		{
			switch( m_state )
			{
			case State_Idle:
				if( m_flags.isSet( Flag_ReceivedPacket ) )
				{
					running = false;
				}
				else
				{
					running = updateReceiveHeader();
				}
				break;

			case State_ReceivingData:
				running = updateReceiveData();
				break;

			case State_Send:
				running = updateSend();
				break;

			case State_WaitingForAck:
				running = updateWaitForAck();
				break;
			}
		}
	}

	bool PortSerial::openSend( Writer& writer, uintreg size, uintreg endpointId )
	{
		if( m_state != State_Idle )
		{
			return false;
		}

		writer.set( m_buffer, sizeof( m_buffer ) - 1u );
		return true;
	}

	void PortSerial::closeSend( Writer& writer, uintreg endpointId )
	{
		m_counter = 0u;
		m_lastSendId = (m_lastSendId + 1u) % 32u;

		const uint8 size = uint8( sizeof( m_buffer ) - writer.getRemainingSize() - 1u );
		writeSendHeader( size, Type_Data, m_lastSendId );
		m_buffer[ size ] = calculateChecksum( m_sendHeader );

		m_state = State_Send;
	}

	bool PortSerial::openReceived( Reader& reader, uintreg& endpointId )
	{
		if( !m_flags.isSet( Flag_ReceivedPacket ) )
		{
			return false;
		}

		endpointId = 0u;
		reader.set( m_buffer, getSizeFromHeader( m_receiveHeader ) );
		return true;
	}

	void PortSerial::closeReceived( Reader& reader, uintreg endpointId )
	{
		m_counter = 0u;
		m_state = State_Idle;
		m_flags.unset( Flag_ReceivedPacket );
	}

	Flags8< PortFlag > PortSerial::getFlags() const
	{
		Flags8< PortFlag > flags;
		flags |= PortFlag_SingleEndpoint;
		flags |= PortFlag_Reliable;
		return flags;
	}

	uint16 PortSerial::getMagicFromHeader( const Header& header ) const
	{
		const uint16 magic = ( header[ 0u ] << 8u ) | ( ( header[ 1u ] & 0x0fu ) << 4u );
		return ( magic & 0xfff0u );
	}

	uint8 PortSerial::getSizeFromHeader( const Header& header ) const
	{
		uint8 dataSize = ( header[ 1u ] & 0xf0u ) >> 3u;
		dataSize |= header[ 2u ] & 0x01u;
		return dataSize;
	}

	PortSerial::Type PortSerial::getTypeFromHeader( const Header& header ) const
	{
		return ( Type )( ( header[ 2u ] & 0x06u ) >> 1u );
	}

	uint8 PortSerial::getIdFromHeader( const Header& header ) const
	{
		return ( header[ 2u ] & 0xf8u ) >> 3u;
	}

	void PortSerial::writeSendHeader( uint8 size, Type type, uint8 id )
	{
		m_sendHeader[ 0u ] = 0x42u;
		m_sendHeader[ 1u ] = 0x0bu | ( ( ( size >> 1u ) & 0x0fu ) << 4u );
		m_sendHeader[ 2u ] = ( size & 0x01u ) | ( type << 1u ) | ( ( id & 0x1fu ) << 3u );
	}

	bool PortSerial::updateReceiveHeader()
	{
		while( Serial1.available() )
		{
			const uint8 byte = Serial1.read();
			m_receiveHeader[ m_counter++ ] = byte;

			static const char s_aHexChars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
			char hexBuffer[] = { s_aHexChars[ ( byte & 0xf0u ) >> 4u ], s_aHexChars[ byte & 0x0fu ], 0u };
			char charBuffer[] ={ ' ', byte, 0u };
			m_hexTrace += hexBuffer;
			m_charTrace += charBuffer;
#elif CONCT_ENABLED( CONCT_PLATFORM_ARDUINO )
			Serial.write( s_aHexChars[ ( byte & 0xf0u ) >> 4u ] );
			Serial.write( s_aHexChars[ byte & 0x0fu ] );
#endif

			if( m_counter == 2u )
			{
				if( getMagicFromHeader( m_receiveHeader ) != 0x42b0u )
				{
					m_receiveHeader[ 0u ] = m_receiveHeader[ 1u ];
					m_counter = 1u;
				}
			}
			else if( m_counter == 3u )
			{
#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
				trace::write( m_hexTrace );
				trace::write( m_charTrace );
				m_hexTrace.clear();
				m_charTrace.clear();
#elif CONCT_ENABLED( CONCT_PLATFORM_ARDUINO )
				Serial.write( '\n' );
#endif

				const Type packetType = getTypeFromHeader( m_receiveHeader );
				if( packetType == Type_Hello )
				{
					m_flags.set( Flag_ConnectionReset );
					sendAck( getIdFromHeader( m_receiveHeader ) );
					return true;
				}
				else if( packetType == Type_Data )
				{
					if( m_state == State_WaitingForAck )
					{
						m_counter = 0u;
						continue;
					}

					m_counter = 0u;
					m_state = State_ReceivingData;
					return true;
				}
				else if( packetType == Type_Ack )
				{
					if( getIdFromHeader( m_receiveHeader ) == m_lastSendId )
					{
						m_counter = 0u;
						m_state = State_Idle;
					}
					else
					{
						m_flags.set( Flag_ConnectionReset );
						sendHello();
					}
					return true;
				}
			}
			CONCT_ASSERT( m_counter < 3u );
		}

		return false;
	}

	bool PortSerial::updateReceiveData()
	{
		const uint8 dataSize = getSizeFromHeader( m_receiveHeader );
		while( Serial1.available() )
		{
			if( m_counter < dataSize )
			{
				m_buffer[ m_counter++ ] = Serial1.read();
			}
			else
			{
				const uint8 receivedChecksum = Serial1.read();
				const uint8 localChecksum = calculateChecksum( m_receiveHeader );

				const uint8 id = getIdFromHeader( m_receiveHeader );
				if( receivedChecksum == localChecksum )
				{
					if( id != m_lastLastReceivedId )
					{
						m_flags.set( Flag_ReceivedPacket );
					}
					m_lastLastReceivedId = id;
					sendAck( getIdFromHeader( m_receiveHeader ) );
					return true;
				}
				else
				{
					m_counter = 0u;
					m_state = State_Idle;
					return true;
				}
			}
		}

		return false;
	}

	bool PortSerial::updateSend()
	{
		for( uintreg i = 0u; i < sizeof( m_sendHeader ); ++i )
		{
			Serial1.write( m_sendHeader[ i ] );
		}

		const uint8 dataSize = getSizeFromHeader( m_sendHeader );
		for( uintreg i = 0u; i <= dataSize; ++i )
		{
			Serial1.write( m_buffer[ i ] );
		}

#if CONCT_ENABLED( CONCT_PLATFORM_ARDUINO )
		while( !( UCSR1A & ( 1 << UDRE1 ) ) )  // Wait for empty transmit buffer
		{
			UCSR1A |= 1 << TXC1;  // mark transmission not complete
		}
		while( !( UCSR1A & ( 1 << TXC1 ) ) );   // Wait for the transmission to complete
#endif

		m_lastSendTime = millis();

		if( getTypeFromHeader( m_sendHeader ) != Type_Ack )
		{
			m_counter = 0u;
			m_state = State_WaitingForAck;
			return false;
		}

		m_counter = 0u;
		m_state = State_Idle;
		return false;
	}

	bool PortSerial::updateWaitForAck()
	{
		const uint32 currentTime = millis();
		if( currentTime < m_lastSendTime || m_lastSendTime + s_resendTime < currentTime ) // first condition is for overflow
		{
			m_counter = 0u;
			m_state = State_Send;
			return true;
		}

		if( updateReceiveHeader() )
		{
			return true;
		}

		return false;
	}

	void PortSerial::sendHello()
	{
		writeSendHeader( 0u, Type_Hello, 0u );

		m_lastSendId	= 0u;
		m_counter		= 0u;
		m_state			= State_Send;
	}

	void PortSerial::sendAck( uint8 packetId )
	{
		writeSendHeader( 0u, Type_Ack, packetId );

		m_counter		= 0u;
		m_state			= State_Send;
	}

	uint8 PortSerial::calculateChecksum( const Header& header ) const
	{
		// source: https://stackoverflow.com/questions/13491700/8-bit-fletcher-checksum-of-16-byte-data
		uint32 sum1 = 1;
		uint32 sum2 = 0;

		const uint8* pData = header;
		const uint8* pDataEnd = pData + sizeof( header );
		while( pData < pDataEnd )
		{
			sum2 += sum1 += *pData++;
		}

		pData = m_buffer;
		pDataEnd = pData + getSizeFromHeader( header );
		while( pData < pDataEnd )
		{
			sum2 += sum1 += *pData++;
		}

		sum1 %= 15u;
		sum2 %= 15u;

		// bit shift modulo 15
		//k = ( k >> 16 ) + ( k & 0xffff );
		//k = ( k >> 8 ) + ( k & 0xff );
		//k = ( k >> 4 ) + ( k & 0xf );
		//k = ( k >> 4 ) + ( k & 0xf );
		//if( k > 14 )
		//{
		//	k -= 15;
		//}

		return ( sum2 << 4u ) + sum1;
	}
}
