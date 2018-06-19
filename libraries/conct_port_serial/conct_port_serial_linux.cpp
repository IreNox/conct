#include "conct_port_serial_linux.h"

#if CONCT_ENABLED( CONCT_PLATFORM_LINUX )

#include "conct_dynamic_string.h"
#include "conct_memory.h"
#include "conct_trace.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

namespace conct
{
	bool Serial::setup( const DynamicString& portName )
	{
		m_fileDescriptor = open( portName.toConstCharPointer(), O_RDWR | O_NOCTTY | O_SYNC );
		if( m_fileDescriptor < 0 )
		{
			trace::write( "open of '"_s + portName + "' failed with error: "_s + strerror( errno ) + "\n" );
			return false;
		}

		return true;
	}

	bool Serial::begin( int speed )
	{
		const bool should_block = true;

		termios tty;
		memory::zero( tty );
		if( tcgetattr( m_fileDescriptor, &tty ) != 0 )
		{
			trace::write( "tcgetattr failed with error: "_s + strerror( errno ) + "\n" );
			return false;
		}

		cfsetospeed( &tty, speed );
		cfsetispeed( &tty, speed );

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

		if( tcsetattr( m_fileDescriptor, TCSANOW, &tty ) != 0 )
		{
			trace::write( "tcsetattr failed with error: "_s + strerror( errno ) + "\n" );
			return false;
		}

		memory::zero( tty );
		if( tcgetattr( m_fileDescriptor, &tty ) != 0 )
		{
			trace::write( "tcgetattr failed with error: "_s + strerror( errno ) + "\n" );
			return false;
		}

		tty.c_cc[ VMIN ]  = should_block ? 1 : 0;
		tty.c_cc[ VTIME ] = 5;            // 0.5 seconds read timeout

		if( tcsetattr( m_fileDescriptor, TCSANOW, &tty ) != 0 )
		{
			trace::write( "tcsetattr failed with error: "_s + strerror( errno ) + "\n" );
			return false;
		}

		return true;
	}

	void Serial::flush()
	{
		tcdrain( m_fileDescriptor );
	}

	int Serial::available()
	{
		int availableBytes = 0u;
		ioctl( m_fileDescriptor, FIONREAD, &availableBytes );
		return availableBytes;
	}

	int Serial::read()
	{
		uint8 byte = 0u;
		if( readData( &byte, sizeof( byte ) ) == 0 )
		{
			return -1;
		}

		return int( byte );
	}

	uintreg Serial::readData( void* pBuffer, uintreg bufferLength )
	{
		int readResult = ::read( m_fileDescriptor, pBuffer, bufferLength );
		if( readResult >= 0 )
		{
			return uintreg( bufferLength );
		}

		trace::write( "serial read failed with error: "_s + strerror( errno ) + "\n" );
		return 0u;
	}

	void Serial::write( uint8 data )
	{
		const int writeResult = ::write( m_fileDescriptor, &data, sizeof( uint8 ) );
		if( writeResult < 0 )
		{
			trace::write( "serial write failed with error: "_s + strerror( errno ) + "\n" );
		}
	}
}

#endif