#include "conct_port_serial.h"

#if CONCT_DISABLED( CONCT_PLATFORM_ARDUINO )
#	include "conct_dynamic_string.h"
#	include "conct_memory.h"
#	include "conct_trace.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

namespace conct
{
	static const uintreg s_serialSpeed = 115200;

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
	void PortSerial::setConfig( const PortSerialConfig& config )
	{
		m_config = config;
	}
#endif

	void PortSerial::setup()
	{
		m_mode		= Mode_Receive;
		m_lastAckId	= 0xffffu;
		m_counter	= 0u;

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )
		Serial1.setup( m_config.portName );
#endif
		Serial1.begin( s_serialSpeed );
	}

	void PortSerial::loop()
	{
		if( m_mode == Mode_Receive )
		{
			while( Serial1.available() )
			{
				m_headerBuffer[ m_counter++ ] = Serial1.read();

				if( m_counter == 2u )
				{
					const uint16 magic = *( uint16* )m_headerBuffer;
					if( magic & 0xfff0u != 0x42b0u )
					{
						m_headerBuffer[ 0u ] = m_headerBuffer[ 1u ];
						m_counter = 1u;
					}
				}
				else if( m_counter == 3u )
				{
					m_remaining = ( m_headerBuffer[ 1u ] & 0xf0u ) >> 3u;
					m_remaining |= m_headerBuffer[ 2u ] & 0x1;

					uintreg type = ( m_headerBuffer[ 2u ] & 0x06u ) >> 1u;
					if( type == Type_Hello )
					{
						m_lastAckId = 0u;
						m_mode = Mode_SendAck;
						break;
					}
				}
			}
		}
	}

	bool PortSerial::openSend( Writer& writer, uintreg size, uintreg endpointId )
	{
		return false;
	}

	void PortSerial::closeSend( Writer& writer, uintreg endpointId )
	{

	}

	bool PortSerial::openReceived( Reader& reader, uintreg& endpointId )
	{
		return false;
	}

	void PortSerial::closeReceived( Reader& reader, uintreg endpointId )
	{

	}

	Flags8< PortFlag > PortSerial::getFlags()
	{
		Flags8< PortFlag > flags;
		flags |= PortFlag_SingleEndpoint;
		flags |= PortFlag_Reliable;
		return flags;
	}
}
