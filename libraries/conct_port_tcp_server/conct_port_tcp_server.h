#pragma once

#include "conct_port.h"

#include "conct_vector.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <WinSock2.h>
#	include <ws2ipdef.h>
#elif CONCT_ENABLED( CONCT_PLATFORM_LINUX )
#	include <arpa/inet.h>
#endif

namespace conct
{
#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
	typedef uintreg SocketType;
#else
	typedef int SocketType;
#endif

	class PortTcpServer : public Port
	{
	public:

		virtual void				setup() CONCT_OVERRIDE_FINAL;
		virtual void				loop() CONCT_OVERRIDE_FINAL;

		virtual bool				openSend( Writer& writer, uintreg size, uintreg endpointId ) CONCT_OVERRIDE_FINAL;
		virtual void				closeSend( Writer& writer, uintreg endpointId ) CONCT_OVERRIDE_FINAL;

		virtual bool				openReceived( Reader& reader, uintreg& endpointId ) CONCT_OVERRIDE_FINAL;
		virtual void				closeReceived( Reader& reader, uintreg endpointId ) CONCT_OVERRIDE_FINAL;

		virtual Flags8< PortFlag >	getFlags() CONCT_OVERRIDE_FINAL;

	private:

		struct Connection
		{
			SocketType				socket;
			sockaddr_in6			address;

			Vector< uint8 >			sendData;
			Vector< uint8 >			receiveData;
		};

		SocketType					m_socket;

		Vector< Connection >		m_connections;

		void						addConnection( uintreg socket, const sockaddr_in6& address );
		bool						updateConnection( Connection& connection );
	};
}