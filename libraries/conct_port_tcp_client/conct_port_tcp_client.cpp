#include "conct_port_tcp_client.h"

#include "conct_memory.h"
#include "conct_reader.h"
#include "conct_string_tools.h"
#include "conct_trace.h"
#include "conct_writer.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <WinSock2.h>
#	include <WS2tcpip.h>
#	include <ws2ipdef.h>
#elif CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
#	include <arpa/inet.h>
#	include <errno.h>
#	include <fcntl.h>
#	include <netdb.h>
#	include <sys/socket.h>
#endif

#include <stdio.h>

namespace conct
{
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

	bool PortTcpClient::setup( const PortTcpClientParameters& parameters )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		const WORD requestedVersion = MAKEWORD( 2, 2 );
		WSADATA wsaData;
		WSAStartup( requestedVersion, &wsaData );
#endif

		m_socket = socket( AF_INET6, SOCK_STREAM, 0 );
		if( m_socket == -1 )
		{
			const int error = getLastError();
			trace::write( "Failed to create Socket. Error: "_s + strerror( error ) );
			return false;
		}

		// set non blocking
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		unsigned long nonBlocking = 1;
		if( ioctlsocket( m_socket, FIONBIO, &nonBlocking ) == SOCKET_ERROR )
		{
			trace::write( "Failed to set non blocking mode." );
			return false;
		}
#else
		const int flags = fcntl( m_socket, F_GETFL, 0 );
		if( fcntl( m_socket, F_SETFL, flags | O_NONBLOCK ) == -1 )
		{
			const int error = getLastError();
			trace::write( "Failed to set non blocking mode. Error: "_s + strerror( error ) );
			return false;
		}
#endif

		addrinfo addressHint;
		memory::zero( addressHint );

		addressHint.ai_socktype = SOCK_STREAM;
		addressHint.ai_flags = AI_ADDRCONFIG;
		addressHint.ai_family = AF_INET6;
		addressHint.ai_protocol = IPPROTO_TCP;

		addrinfo* pAddressResults = nullptr;
		const int result = getaddrinfo( parameters.serverHost.toConstCharPointer(), nullptr, &addressHint, &pAddressResults );
		if( result != 0 )
		{
			const char* pErrorName = "";
#if CONCT_DISABLED( CONCT_PLATFORM_WINDOWS )
			if( result == EAI_SYSTEM )
			{
				const int error = getLastError();
				pErrorName = strerror( error );
			}
			else
			{
				pErrorName = gai_strerror( result );
			}
#endif
			trace::write( "Failed to get address. Error: "_s + pErrorName );
			return false;
		}

		const addrinfo& addressResult = pAddressResults[ 0u ];
		m_serverAddress = *(sockaddr_in6*)addressResult.ai_addr;
		m_serverAddress.sin6_port = htons( parameters.serverPort );

		freeaddrinfo( pAddressResults );
		return true;
		//memory::zero( m_serverAddress );
		//m_serverAddress.sin6_family = AF_INET6;
		//inet_pton( AF_INET6, parameters.targetHost.toConstCharPointer(), &m_serverAddress.sin6_addr );
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
					trace::write( "Couldn't connect to server. Error: "_s + strerror( error ) );
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
			trace::write( "Couldn't get socket error code. Error: "_s + strerror( retval ) );
			m_connectionLost = true;
			return;
		}

		if( error != 0 )
		{
			trace::write( "Socket has an error. Error: "_s + strerror( error ) );
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
