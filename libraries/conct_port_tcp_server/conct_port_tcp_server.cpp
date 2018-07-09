#include "conct_port_tcp_server.h"

#include "conct_dynamic_string.h"
#include "conct_memory.h"
#include "conct_reader.h"
#include "conct_string_tools.h"
#include "conct_trace.h"
#include "conct_writer.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <WinSock2.h>
#	include <WS2tcpip.h>
#	include <ws2ipdef.h>
#elif CONCT_ENABLED( CONCT_PLATFORM_LINUX )
#	include <sys/socket.h>
#	include <fcntl.h>
#endif

#include <stdio.h>
#include <cstring>
#include <errno.h>

namespace conct
{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
	CONCT_STATIC_ASSERT( sizeof( SocketType ) >= sizeof( SOCKET ) );
#else
	CONCT_STATIC_ASSERT( sizeof( SocketType ) >= sizeof( int ) );
#endif

	static const int s_tcpPort = 5489;
	static const SocketType InvalidSocket = ( SocketType )-1;

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

	bool PortTcpServer::popConnectionReset( uintreg& endpointId )
	{
		if( m_brokenConnections.isEmpty() )
		{
			return false;
		}

		endpointId = m_brokenConnections.getBack();
		m_brokenConnections.popBack();

		return true;
	}

	bool PortTcpServer::setup()
	{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		const WORD requestedVersion = MAKEWORD( 2, 2 );
		WSADATA wsaData;
		WSAStartup( requestedVersion, &wsaData );
#endif

		m_socket = socket( AF_INET6, SOCK_STREAM, 0 );
		if( m_socket == -1 )
		{
			return false;
		}

		// reuse address
		int reuseaddr = 1;
		setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, ( const char* )&reuseaddr, sizeof( reuseaddr ) );

		// set non blocking
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		unsigned long nonBlocking = 1;
		if( ioctlsocket( m_socket, FIONBIO, &nonBlocking ) == SOCKET_ERROR )
		{
			return false;
		}
#else
		const int flags = fcntl( m_socket, F_GETFL, 0 );
		if( fcntl( m_socket, F_SETFL, flags | O_NONBLOCK ) == -1 )
		{
			return false;
		}
#endif

		sockaddr_in6 address;
		memory::zero( address );
		address.sin6_family = AF_INET6;
		address.sin6_port = htons( s_tcpPort );
		address.sin6_addr = in6addr_any;
		if( bind( m_socket, (const sockaddr*)&address, sizeof( address ) ) != 0 )
		{
			const int error = getLastError();
			trace::write( "Bind failed with an error. Error: "_s + strerror( error ) + "\n" );
			return false;
		}

		if( listen( m_socket, 5 ) != 0 )
		{
			const int error = getLastError();
			trace::write( "Listen failed with an error. Error: "_s + strerror( error ) + "\n" );
			return false;
		}

		trace::write( "Listen on port: "_s + string_tools::toString( s_tcpPort ) + "\n" );
		return true;
	}

	void PortTcpServer::loop()
	{
		sockaddr_in6 address;
		socklen_t addressLength = sizeof( address );
		SocketType clientSocket = accept( m_socket, (sockaddr*)&address, &addressLength );
		if( clientSocket != InvalidSocket )
		{
			addConnection( clientSocket, address );
		}

		for( uintreg i = 0u; i < m_connections.getLength(); ++i )
		{
			Connection& connection = m_connections[ i ];
			if( connection.socket == InvalidSocket )
			{
				continue;
			}

			if( !updateConnection( connection ) )
			{
				trace::write( "Connection closed: "_s + string_tools::toString( connection.socket ) + "\n" );

				m_brokenConnections.pushBack( i );
				shutdown( connection.socket, 0 );

				connection.receiveData.clear();
				connection.sendData.clear();
				connection.socket = InvalidSocket;
			}
		}
	}

	bool PortTcpServer::openSend( Writer& writer, uintreg size, uintreg endpointId )
	{
		if( endpointId >= m_connections.getLength() )
		{
			return false;
		}

		Connection& connection = m_connections[ endpointId ];

		connection.sendData.setLength( connection.sendData.getLength() + size );
		writer.set( connection.sendData.getEnd() - size, size );
		return true;
	}

	void PortTcpServer::closeSend( Writer& writer, uintreg endpointId )
	{
		CONCT_ASSERT( writer.isEnd() );
	}

	bool PortTcpServer::openReceived( Reader& reader, uintreg& endpointId )
	{
		for( uintreg i = 0u; i < m_connections.getLength(); ++i )
		{
			Connection& connection = m_connections[ i ];
			if( connection.receiveData.isEmpty() )
			{
				continue;
			}

			endpointId = i;
			reader.set( connection.receiveData.getData(), connection.receiveData.getLength() );
			return true;
		}

		return false;
	}

	void PortTcpServer::closeReceived( Reader& reader, uintreg endpointId )
	{
		CONCT_ASSERT( reader.isEnd() );

		Connection& connection = m_connections[ endpointId ];
		connection.receiveData.clear();
	}

	Flags8< PortFlag > PortTcpServer::getFlags()
	{
		Flags8< PortFlag > flags;
		flags |= PortFlag_MultiEndpoint;
		flags |= PortFlag_Reliable;
		return flags;
	}

	void PortTcpServer::addConnection( uintreg socket, const sockaddr_in6& address )
	{
#if CONCT_ENABLED( CONCT_PLATFORM_LINUX ) || CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
		const int flags = fcntl( m_socket, F_GETFL, 0 );
		if( fcntl( socket, F_SETFL, flags | O_NONBLOCK ) == -1 )
		{
			shutdown( socket, 0 );
			return;
		}
#endif

		Connection* pConnection = nullptr;
		for( size_t i = 0u; i < m_connections.getLength(); ++i )
		{
			Connection& connection = m_connections[ i ];
			if( connection.socket != InvalidSocket )
			{
				continue;
			}

			pConnection = &connection;
			break;
		}

		if( pConnection == nullptr )
		{
			pConnection	= &m_connections.pushBack();
		}

		pConnection->socket		= socket;
		pConnection->address	= address;

		trace::write( "Connection added: "_s + string_tools::toString( socket ) + "\n" );
	}

	bool PortTcpServer::updateConnection( Connection& connection )
	{
		int error = 0;
		socklen_t len = sizeof( error );
		int retval = getsockopt( connection.socket, SOL_SOCKET, SO_ERROR, (char*)&error, &len );
		if( retval != 0 )
		{
			trace::write( "Couldn't get socket error code. Error: "_s + strerror( retval ) + "\n" );
			return false;
		}

		if( error != 0 )
		{
			trace::write( "Socket has an error. Error: "_s + strerror( error ) + "\n" );
			return false;
		}

		if( !connection.sendData.isEmpty() )
		{
			const int sendResult = send( connection.socket, ( const char* )connection.sendData.getData(), ( int )connection.sendData.getLength(), 0u );
			if( sendResult >= 0 )
			{
				connection.sendData.clear();
			}
			else
			{
				const int error = getLastError();
				trace::write( "Send failed with an error. Error: "_s + strerror( error ) + "\n" );
				return false;
			}
		}

		int receiveResult = 0u;
		do
		{
			connection.receiveData.reserve( connection.receiveData.getLength() + 2048u );
			receiveResult = recv( connection.socket, ( char* )connection.receiveData.getEnd(), 2048, 0 );
			if( receiveResult < 0 )
			{
				const int error = getLastError();
				if( error == EAGAIN || error == ErrorWouldBlock )
				{
					break;
				}

				trace::write( "Receive failed with an error. Error: "_s + strerror( error ) + "\n" );
				return false;
			}

			connection.receiveData.setLength( connection.receiveData.getLength() + receiveResult );
		}
		while( receiveResult == 2048 );

		return true;
	}
}
