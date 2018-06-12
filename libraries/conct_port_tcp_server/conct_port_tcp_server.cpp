#include "conct_port_tcp_server.h"

#include "conct_memory.h"
#include "conct_reader.h"
#include "conct_writer.h"

//#include <sys/socket.h>
//#include <arpa/inet.h>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <ws2ipdef.h>

#include <stdio.h>

namespace conct
{
	CONCT_STATIC_ASSERT( sizeof( uintreg ) == sizeof( SOCKET ) );

	static const int s_tcpPort = 5489;

	void PortTcpServer::setup()
	{
		const WORD requestedVersion = MAKEWORD( 2, 2 );
		WSADATA wsaData;
		WSAStartup( requestedVersion, &wsaData );

		m_socket = socket( AF_INET6, SOCK_STREAM, 0 );
		if( m_socket == -1 )
		{
			return;
		}

		// reuse address
		int reuseaddr = 1;
		setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, ( const char* )&reuseaddr, sizeof( reuseaddr ) );

		// set non blocking
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
		unsigned long nonBlocking = 1;
		if( ioctlsocket( m_socket, FIONBIO, &nonBlocking ) == SOCKET_ERROR )
		{
			return;
		}
#else
		const int flags = fcntl( socket, F_GETFL, 0 );
		if( fcntl( m_socket, F_SETFL, flags | O_NONBLOCK ) == -1 )
		{
			return;
		}
#endif

		sockaddr_in6 address;
		memory::zero( address );
		address.sin6_family = AF_INET6;
		address.sin6_port = htons( 49351 );
		address.sin6_addr = in6addr_any;
		if( bind( m_socket, (const sockaddr*)&address, sizeof( address ) ) != 0 )
		{
			const int test = WSAGetLastError();
			printf( "%u\n", test );
			return;
		}

		if( listen( m_socket, 5 ) != 0 )
		{
			const int test = WSAGetLastError();
			printf( "%u\n", test );
			return;
		}
	}

	void PortTcpServer::loop()
	{
		sockaddr_in6 address;
		int addressLength = sizeof( address );
		SOCKET clientSocket = accept( m_socket, (sockaddr*)&address, &addressLength );
		if( clientSocket != INVALID_SOCKET )
		{
			addConnection( clientSocket, address );
		}

		for( Connection& connection : m_connections )
		{
			updateConnection( connection );
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
		Connection& connection = m_connections.pushBack();
		connection.socket	= socket;
		connection.address	= address;
	}

	void PortTcpServer::updateConnection( Connection& connection )
	{
		const int sendResult = send( connection.socket, ( const char* )connection.sendData.getData(), (int)connection.sendData.getLength(), 0u );
		if( sendResult >= 0 )
		{
			connection.sendData.clear();
		}
		else
		{
			const int test = WSAGetLastError();
			if( test != WSAEWOULDBLOCK )
			{
				printf( "%u\n", test );
			}
		}

		int receivedBytes = 0u;
		do
		{
			connection.receiveData.reserve( connection.receiveData.getLength() + 2048u );
			receivedBytes = recv( connection.socket, ( char* )connection.receiveData.getEnd(), 2048, 0u );
			if( receivedBytes < 0 )
			{
				const int test = WSAGetLastError();
				if( test != WSAEWOULDBLOCK )
				{
					printf( "%u\n", test );
				}
				break;
			}

			connection.receiveData.setLength( connection.receiveData.getLength() + receivedBytes );
		}
		while( receivedBytes == 2048 );
	}
}
