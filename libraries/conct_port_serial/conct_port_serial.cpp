#include "conct_port_serial.h"

#include "conct_reader.h"
#include "conct_writer.h"

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
#	include "conct_dynamic_string.h"
#	include "conct_memory.h"
#	include "conct_trace.h"

#	include <tiny_gpio/tiny_gpio.h>
#endif

namespace conct
{
	static const uintreg s_serialSendPin = CONCT_ENABLED( CONCT_PLATFORM_LINUX ) ? 1u : 5u;
	static const uintreg s_serialSpeed = 115200;

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

	void PortSerial::setConfig( const PortSerialConfig& config )
	{
		m_config = config;
	}
#endif

	void PortSerial::setup()
	{
		m_state			= State_Idle;
		m_lastSendId	= 0u;
		m_counter		= 0u;

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

		writer.set( m_buffer, sizeof( m_buffer ) );
		return true;
	}

	void PortSerial::closeSend( Writer& writer, uintreg endpointId )
	{
		m_counter = 0u;
		m_lastSendId++;
		writeHeader( sizeof( m_buffer ) - writer.getRemainingSize(), Type_Data, m_lastSendId );
		m_state = State_Send;
	}

	bool PortSerial::openReceived( Reader& reader, uintreg& endpointId )
	{
		if( m_state != State_ReceivedPacket )
		{
			return false;
		}

		reader.set( m_buffer, getSizeFromHeader() );
		return true;
	}

	void PortSerial::closeReceived( Reader& reader, uintreg endpointId )
	{
		sendAck( getIdFromHeader() );
	}

	Flags8< PortFlag > PortSerial::getFlags()
	{
		Flags8< PortFlag > flags;
		flags |= PortFlag_SingleEndpoint;
		flags |= PortFlag_Reliable;
		return flags;
	}

	uint16 PortSerial::getMagicFromHeader() const
	{
		const uint16 magic = *( uint16* )m_headerBuffer;
		return ( magic & 0xfff0u );
	}

	uint8 PortSerial::getSizeFromHeader() const
	{
		uint8 dataSize = ( m_headerBuffer[ 1u ] & 0xf0u ) >> 4u;
		dataSize |= m_headerBuffer[ 2u ] & 0x01u;
		return dataSize;
	}

	PortSerial::Type PortSerial::getTypeFromHeader() const
	{
		return ( Type )( ( m_headerBuffer[ 2u ] & 0x06u ) >> 1u );
	}

	uint8 PortSerial::getIdFromHeader() const
	{
		return ( m_headerBuffer[ 2u ] & 0x8fu ) >> 3u;
	}

	void PortSerial::writeHeader( uint8 size, Type type, uint8 id )
	{
		m_headerBuffer[ 0u ] = 0x42u;
		m_headerBuffer[ 1u ] = 0xb0u | ( ( ( size >> 1u ) & 0x0fu ) << 4u );
		m_headerBuffer[ 2u ] = ( size & 0x01u ) | ( type << 1u ) | ( ( id & 0x1fu ) << 3u );
	}

	bool PortSerial::updateReceiveHeader()
	{
		while( Serial1.available() )
		{
			m_headerBuffer[ m_counter++ ] = Serial1.read();

			if( m_counter == 2u )
			{
				if( getMagicFromHeader() != 0x42b0u )
				{
					m_headerBuffer[ 0u ] = m_headerBuffer[ 1u ];
					m_counter = 1u;
				}
			}
			else if( m_counter == 3u )
			{
				const Type packetType = getTypeFromHeader();
				if( packetType == Type_Hello )
				{
					sendAck( getIdFromHeader() );
					return true;
				}
				else if( packetType == Type_Data )
				{
					CONCT_ASSERT( getSizeFromHeader() > 0u );
					m_counter = 0u;
					m_state = State_ReceivingData;
					return true;
				}
				else if( packetType == Type_Ack )
				{
					if( getIdFromHeader() == m_lastSendId )
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
		const uint8 dataSize = getSizeFromHeader();
		while( Serial1.available() )
		{
			if( m_counter < dataSize )
			{
				m_headerBuffer[ m_counter++ ] = Serial1.read();
			}
			else
			{
				const uint8 receivedChecksum = Serial1.read();
				const uint8 localChecksum = calculateChecksum();

				if( receivedChecksum == localChecksum )
				{
					sendAck( getIdFromHeader() );
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

		for( uintreg i = 0u; i < sizeof( m_headerBuffer ); ++i )
		{
			Serial1.write( m_headerBuffer[ i ] );
		}

		const uint8 dataSize = getSizeFromHeader();
		for( uintreg i = 0u; i < dataSize; ++i )
		{
			Serial1.write( m_buffer[ i ] );
		}

		digitalWrite( s_serialSendPin, LOW );

		if( getTypeFromHeader() != Type_Ack )
		{
			m_counter = 0u;
			m_state = State_WaitingForAck;
			return false;
		}

		m_counter = 0u;
		m_state = State_Idle;
		return true;
	}

	bool PortSerial::updateWaitForAck()
	{
		if( !updateReceiveHeader() )
		{
			return false;
		}

		return false;
	}

	void PortSerial::sendHello()
	{
		writeHeader( 0u, Type_Hello, 0u );

		m_counter = 0u;
		m_state = State_Send;
	}

	void PortSerial::sendAck( uintreg packetId )
	{
		writeHeader( 0u, Type_Ack, packetId );

		m_counter = 0u;
		m_state = State_Send;
	}
}
