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

	void PortSerial::setup()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_ARDUINO )
		Serial1.begin( s_serialSpeed );
#else
		int fd = open( "", O_RDWR | O_NOCTTY | O_SYNC );

		termios tty;
		memory::zero( tty );
		if( tcgetattr( fd, &tty ) != 0 )
		{
			trace::write( "tcgetattr failed with error: "_s + strerror( errno ) + "\n" );
			return;
		}

		cfsetospeed( &tty, s_serialSpeed );
		cfsetispeed( &tty, s_serialSpeed );

		tty.c_cflag = ( tty.c_cflag & ~CSIZE ) | CS8;     // 8-bit chars
														  // disable IGNBRK for mismatched speed tests; otherwise receive break
														  // as \000 chars
		tty.c_iflag &= ~IGNBRK;         // disable break processing
		tty.c_lflag = 0;                // no signaling chars, no echo,
										// no canonical processing
		tty.c_oflag = 0;                // no remapping, no delays
		tty.c_cc[ VMIN ]  = 0;            // read doesn't block
		tty.c_cc[ VTIME ] = 5;            // 0.5 seconds read timeout

		tty.c_iflag &= ~( IXON | IXOFF | IXANY ); // shut off xon/xoff ctrl

		tty.c_cflag |= ( CLOCAL | CREAD );// ignore modem controls,
										  // enable reading
		tty.c_cflag &= ~( PARENB | PARODD );      // shut off parity
		tty.c_cflag &= ~CSTOPB;
		tty.c_cflag &= ~CRTSCTS;

		if( tcsetattr( fd, TCSANOW, &tty ) != 0 )
		{
			trace::write( "tcsetattr failed with error: "_s + strerror( errno ) + "\n" );
			return;
		}
#endif
	}

	void PortSerial::loop()
	{

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
