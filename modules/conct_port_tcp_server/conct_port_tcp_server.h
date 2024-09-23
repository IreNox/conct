#pragma once

#include "conct_port.h"

#include <tiki/tiki_dynamic_string.h>

#if TIKI_ENABLED( TIKI_PLATFORM_WINDOWS )
#	include <WinSock2.h>
#	include <ws2ipdef.h>
#elif TIKI_ENABLED( TIKI_PLATFORM_LINUX )
#	include <arpa/inet.h>
#endif

namespace conct
{
#if TIKI_ENABLED( TIKI_PLATFORM_WINDOWS )
	typedef uintreg SocketType;
#else
	typedef int SocketType;
#endif

	struct PortTcpServerParameters
	{
		DynamicString	listenAddress;
		int				listenPort;
	};

	class PortTcpServer : public Port
	{
	public:

		bool						setup( const PortTcpServerParameters& parameters );

		virtual void				getEndpoints( ConstArrayView< uintreg >& endpoints ) override final;
		virtual bool				popConnectionReset( uintreg& endpointId ) override final;

		virtual void				loop() override final;

		virtual bool				openSend( Writer& writer, uintreg size, uintreg endpointId ) override final;
		virtual void				closeSend( Writer& writer, uintreg endpointId ) override final;

		virtual bool				openReceived( Reader& reader, uintreg& endpointId ) override final;
		virtual void				closeReceived( Reader& reader, uintreg endpointId ) override final;

		virtual Flags8< PortFlag >	getFlags() const override final;

	private:

		struct Connection
		{
			SocketType				socket;
			sockaddr_in6			address;

			DynamicArray< uint8 >	sendData;
			DynamicArray< uint8 >	receiveData;
		};

		SocketType					m_socket;

		DynamicArray< Connection >	m_connections;
		DynamicArray< uintreg >		m_brokenConnections;
		DynamicArray< uintreg >		m_connectedConnections;

		void						addConnection( uintreg socket, const sockaddr_in6& address );
		bool						updateConnection( Connection& connection );
	};
}