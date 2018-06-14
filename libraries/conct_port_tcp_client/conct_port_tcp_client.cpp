#include "conct_port_tcp_client.h"

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
	static const int s_tcpPort = 5489;

	PortTcpClient::PortTcpClient()
	{
		m_config.targetHost		= "::1"_s;
		m_config.targetPort		= s_tcpPort;
		m_socket				= INVALID_SOCKET;
	}

	PortTcpClient::~PortTcpClient()
	{
		if( m_socket != INVALID_SOCKET )
		{
			closesocket( m_socket );
		}
	}

	void PortTcpClient::setConfig( const PortTcpClientConfig& config )
	{
		m_config = config;
	}

	void PortTcpClient::setup()
	{
		const WORD requestedVersion = MAKEWORD( 2, 2 );
		WSADATA wsaData;
		WSAStartup( requestedVersion, &wsaData );

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
		inet_pton( AF_INET6, m_config.targetHost.toConstCharPointer(), &address.sin6_addr );

		if( connect( m_socket, (const sockaddr*)&address, sizeof( address ) ) != 0 )
		{
			const int test = WSAGetLastError();
			if( test != WSAEWOULDBLOCK )
			{
				printf( "%u\n", test );
			}
			return;
		}
	}

	void PortTcpClient::loop()
	{
		const int sendResult = send( m_socket, ( const char* )m_sendData.getData(), ( int )m_sendData.getLength(), 0u );
		if( sendResult >= 0 )
		{
			m_sendData.clear();
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
			m_receiveData.reserve( m_receiveData.getLength() + 2048u );
			receivedBytes = recv( m_socket, ( char* )m_receiveData.getEnd(), 2048, 0u );
			if( receivedBytes < 0 )
			{
				const int test = WSAGetLastError();
				if( test != WSAEWOULDBLOCK )
				{
					printf( "%u\n", test );
				}
				break;
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
