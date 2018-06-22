#include "conct_port_tcp_client.h"

#include "conct_memory.h"
#include "conct_reader.h"
#include "conct_writer.h"
#include "conct_trace.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <WinSock2.h>
#	include <WS2tcpip.h>
#	include <ws2ipdef.h>
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
#	include <sys/socket.h>
#	include <arpa/inet.h>
#	include <fcntl.h>
#endif

#include <stdio.h>

namespace conct
{
	static const int s_tcpPort = 5489;
	static const uintreg InvalidSocket = ( uintreg )-1;

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
	static const int ErrorWouldBlock = WSAEWOULDBLOCK;
	static const int ErrorAlreadyInProgress = WSAEALREADY;
#else
	static const int ErrorWouldBlock = EWOULDBLOCK;
	static const int ErrorAlreadyInProgress = EALREADY;
#endif


	int getLastError()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		return WSAGetLastError();
#else
		return errno;
#endif
	}

	PortTcpClient::PortTcpClient()
	{
		m_config.targetHost		= "::1"_s;
		m_config.targetPort		= s_tcpPort;
		m_socket				= InvalidSocket;
		m_connectionLost		= true;
		m_connectionReset		= false;
	}

	PortTcpClient::~PortTcpClient()
	{
		if( m_socket != InvalidSocket )
		{
			shutdown( m_socket, 0 );
		}
	}

	void PortTcpClient::setConfig( const PortTcpClientConfig& config )
	{
		m_config = config;
	}

	bool PortTcpClient::popConnectionReset( uintreg& endpointId )
	{
		if( !m_connectionReset )
		{
			return false;
		}

		m_connectionReset = false;
		return true;
	}

	void PortTcpClient::setup()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		const WORD requestedVersion = MAKEWORD( 2, 2 );
		WSADATA wsaData;
		WSAStartup( requestedVersion, &wsaData );
#endif

		m_socket = socket( AF_INET6, SOCK_STREAM, 0 );
		if( m_socket == -1 )
		{
			return;
		}

		// set non blocking
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		unsigned long nonBlocking = 1;
		if( ioctlsocket( m_socket, FIONBIO, &nonBlocking ) == SOCKET_ERROR )
		{
			return;
		}
#else
		const int flags = fcntl( m_socket, F_GETFL, 0 );
		if( fcntl( m_socket, F_SETFL, flags | O_NONBLOCK ) == -1 )
		{
			return;
		}
#endif


		memory::zero( m_serverAddress );
		m_serverAddress.sin6_family = AF_INET6;
		m_serverAddress.sin6_port = htons( m_config.targetPort );
		inet_pton( AF_INET6, m_config.targetHost.toConstCharPointer(), &m_serverAddress.sin6_addr );
	}

	void PortTcpClient::loop()
	{
		if( m_connectionLost )
		{
			m_connectionReset = true;

			if( connect( m_socket, ( const sockaddr* )&m_serverAddress, sizeof( m_serverAddress ) ) != 0 )
			{
				const int error = getLastError();
				if( error != ErrorWouldBlock && error != ErrorAlreadyInProgress )
				{
					trace::write( "Couldn't connect to server. Error: "_s + strerror( error ) + "\n" );
					m_connectionLost = true;
					return;
				}
			}

			m_connectionLost = false;
		}

		int error = 0;
		socklen_t len = sizeof( error );
		int retval = getsockopt( m_socket, SOL_SOCKET, SO_ERROR, ( char* )&error, &len );
		if( retval != 0 )
		{
			trace::write( "Couldn't get socket error code. Error: "_s + strerror( retval ) + "\n" );
			m_connectionLost = true;
			return;
		}

		if( error != 0 )
		{
			trace::write( "Socket has an error. Error: "_s + strerror( error ) + "\n" );
			m_connectionLost = true;
			return;
		}

		if( !m_sendData.isEmpty() )
		{
			const int sendResult = send( m_socket, ( const char* )m_sendData.getData(), ( int )m_sendData.getLength(), 0u );
			if( sendResult >= 0 )
			{
				m_sendData.clear();
			}
			else
			{
				const int error = getLastError();
				trace::write( "Send failed with an error. Error: "_s + strerror( error ) );
				m_connectionLost = true;
				return;
			}
		}

		int receivedBytes = 0u;
		do
		{
			m_receiveData.reserve( m_receiveData.getLength() + 2048u );
			receivedBytes = recv( m_socket, ( char* )m_receiveData.getEnd(), 2048, 0 );
			if( receivedBytes < 0 )
			{
				const int error = getLastError();
				if( error != EWOULDBLOCK )
				{
					break;
				}

				trace::write( "Receive failed with an error. Error: "_s + strerror( error ) );
				m_connectionLost = true;
				return;
			}

			m_receiveData.setLength( m_receiveData.getLength() + receivedBytes );
		}
		while( receivedBytes == 2048 );
	}

	bool PortTcpClient::openSend( Writer& writer, uintreg size, uintreg endpointId )
	{
		m_sendData.setLength( m_sendData.getLength() + size );
		writer.set( m_sendData.getEnd() - size, size );
		return true;
	}

	void PortTcpClient::closeSend( Writer& writer, uintreg endpointId )
	{
		CONCT_ASSERT( writer.isEnd() );
	}

	bool PortTcpClient::openReceived( Reader& reader, uintreg& endpointId )
	{
		if( m_receiveData.isEmpty() )
		{
			return false;
		}

		endpointId = 0u;
		reader.set( m_receiveData.getData(), m_receiveData.getLength() );
		return true;
	}

	void PortTcpClient::closeReceived( Reader& reader, uintreg endpointId )
	{
		CONCT_ASSERT( reader.isEnd() );

		m_receiveData.clear();
	}

	Flags8< PortFlag > PortTcpClient::getFlags()
	{
		Flags8< PortFlag > flags;
		flags |= PortFlag_SingleEndpoint;
		flags |= PortFlag_Reliable;
		return flags;
	}
}
