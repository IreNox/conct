#include "conct_port_serial.h"

#include "conct_reader.h"
#include "conct_writer.h"

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
#	include "conct_dynamic_string.h"
#	include "conct_memory.h"
#	include "conct_trace.h"

#	include <tiny_gpio/tiny_gpio.h>

#	include <time.h>
#endif

namespace conct
{
	static const uintreg s_serialSendPin = CONCT_ENABLED( CONCT_PLATFORM_LINUX ) ? 1u : 5u;
	static const uintreg s_serialSpeed = 115200;
	static const uintreg s_resendTime = 200;

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
	enum PinMode
	{
		INPUT,
		OUTPUT
	};

	enum PinValue
	{
		LOW,
		HIGH
	};

	void pinMode( uintreg pin, PinMode mode )
	{
		gpioSetMode( pin, mode );
	}

	PinValue digitalRead( uintreg pin )
	{
		return gpioRead( pin ) ? HIGH : LOW;
	}

	void digitalWrite( uintreg pin, PinValue value )
	{
		gpioWrite( pin, value );
	}

	uint32 millis()
	{
		timespec currentTime;
		currentTime.tv_sec = 0;
		currentTime.tv_nsec = 0;
		clock_gettime( CLOCK_MONOTONIC, &currentTime );

		uint32 result = uint32( currentTime.tv_sec ) * 1000u;
		result += uint32( currentTime.tv_nsec ) / 1000000u;
		return result;
	}

	void PortSerial::setConfig( const PortSerialConfig& config )
	{
		m_config = config;
	}
#endif

	bool PortSerial::popConnectionReset( uintreg& endpointId )
	{
		if( m_connectionReset )
		{
			return false;
		}

		endpointId = 0u;

		m_counter = 0u;
		m_state = State_Idle;

		return true;
	}

	void PortSerial::setup()
	{
		m_state				= State_Idle;
		m_lastSendId		= 0u;
		m_counter			= 0u;
		m_connectionReset	= true;

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
		if( gpioInitialise() < 0 )
		{
			return;
		}

		if( !Serial1.setup( m_config.portName ) )
		{
			return;
		}
#endif

		pinMode( s_serialSendPin, OUTPUT );
		Serial1.begin( s_serialSpeed );

		sendHello();
	}

	void PortSerial::loop()
	{
		bool running = true;
		while( running )
		{
			switch( m_state )
			{
			case State_Idle:
			case State_ReceivingHeader:
				running = updateReceiveHeader();
				break;

			case State_ReceivingData:
				running = updateReceiveData();
				break;

			case State_ReceivedPacket:
				running = false;
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
		m_lastSendId++;

		const uintreg size = sizeof( m_buffer ) - writer.getRemainingSize();
		writeSendHeader( size, Type_Data, m_lastSendId );
		m_buffer[ size ] = calculateChecksum( m_sendHeader );

		m_state = State_Send;
	}

	bool PortSerial::openReceived( Reader& reader, uintreg& endpointId )
	{
		if( m_state != State_ReceivedPacket )
		{
			return false;
		}

		reader.set( m_buffer, getSizeFromHeader( m_receiveHeader ) );
		return true;
	}

	void PortSerial::closeReceived( Reader& reader, uintreg endpointId )
	{
		m_counter = 0u;
		m_state = State_Idle;
	}

	Flags8< PortFlag > PortSerial::getFlags()
	{
		Flags8< PortFlag > flags;
		flags |= PortFlag_SingleEndpoint;
		flags |= PortFlag_Reliable;
		return flags;
	}

	uint16 PortSerial::getMagicFromHeader( const Header& header ) const
	{
		const uint16 magic = *( uint16* )header;
		return ( magic & 0xfff0u );
	}

	uint8 PortSerial::getSizeFromHeader( const Header& header ) const
	{
		uint8 dataSize = ( header[ 1u ] & 0xf0u ) >> 4u;
		dataSize |= header[ 2u ] & 0x01u;
		return dataSize;
	}

	PortSerial::Type PortSerial::getTypeFromHeader( const Header& header ) const
	{
		return ( Type )( ( header[ 2u ] & 0x06u ) >> 1u );
	}

	uint8 PortSerial::getIdFromHeader( const Header& header ) const
	{
		return ( header[ 2u ] & 0x8fu ) >> 3u;
	}

	void PortSerial::writeSendHeader( uint8 size, Type type, uint8 id )
	{
		m_sendHeader[ 0u ] = 0x42u;
		m_sendHeader[ 1u ] = 0xb0u | ( ( ( size >> 1u ) & 0x0fu ) << 4u );
		m_sendHeader[ 2u ] = ( size & 0x01u ) | ( type << 1u ) | ( ( id & 0x1fu ) << 3u );
	}

	bool PortSerial::updateReceiveHeader()
	{
		while( Serial1.available() )
		{
			m_receiveHeader[ m_counter++ ] = Serial1.read();

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
				const Type packetType = getTypeFromHeader();
				if( packetType == Type_Hello )
				{
					sendAck( getIdFromHeader( m_receiveHeader ) );
					return true;
				}
				else if( packetType == Type_Data && m_state != State_WaitingForAck )
				{
					CONCT_ASSERT( getSizeFromHeader( m_receiveHeader ) > 0u );
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
						sendHello();
					}
					return true;
				}
			}
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

				if( receivedChecksum == localChecksum )
				{
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
		digitalWrite( s_serialSendPin, HIGH );

		for( uintreg i = 0u; i < sizeof( m_sendHeader ); ++i )
		{
			Serial1.write( m_sendHeader[ i ] );
		}

		const uint8 dataSize = getSizeFromHeader( m_sendHeader );
		for( uintreg i = 0u; i < dataSize; ++i )
		{
			Serial1.write( m_buffer[ i ] );
		}

		digitalWrite( s_serialSendPin, LOW );

		if( getTypeFromHeader( m_sendHeader ) != Type_Ack )
		{
			m_counter = 0u;
			m_lastSendTime = millis();
			m_state = State_WaitingForAck;
			return false;
		}

		m_counter = 0u;
		m_state = State_Idle;
		return true;
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

		m_counter = 0u;
		m_state = State_Send;
		m_connectionReset = true;
	}

	void PortSerial::sendAck( uintreg packetId )
	{
		writeSendHeader( 0u, Type_Ack, packetId );

		m_counter = 0u;
		m_state = State_Send;
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
