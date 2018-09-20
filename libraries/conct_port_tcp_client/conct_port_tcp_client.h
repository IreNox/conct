#pragma once

#include "conct_port.h"

#include "conct_vector.h"
#include "conct_dynamic_string.h"

#if CONCT_ENABLED( CONCT_PLATFORM_WINDOWS )
#	include <WinSock2.h>
#	include <ws2ipdef.h>
#elif CONCT_ENABLED( CONCT_PLATFORM_LINUX ) || CONCT_ENABLED( CONCT_PLATFORM_ANDROID )
#	include <arpa/inet.h>
#endif

namespace conct
{
	struct PortTcpClientParameters
	{
		DynamicString		serverHost;		// IPv6 or Hostname
		int					serverPort;
	};

	class PortTcpClient : public Port
	{
	public:

									PortTcpClient();
		virtual						~PortTcpClient();

		bool						setup( const PortTcpClientParameters& parameters );

		virtual bool				popConnectionReset( uintreg& endpointId ) CONCT_OVERRIDE_FINAL;

		virtual void				loop() CONCT_OVERRIDE_FINAL;

		virtual bool				openSend( Writer& writer, uintreg size, uintreg endpointId ) CONCT_OVERRIDE_FINAL;
		virtual void				closeSend( Writer& writer, uintreg endpointId ) CONCT_OVERRIDE_FINAL;

		virtual bool				openReceived( Reader& reader, uintreg& endpointId ) CONCT_OVERRIDE_FINAL;
		virtual void				closeReceived( Reader& reader, uintreg endpointId ) CONCT_OVERRIDE_FINAL;

		virtual Flags8< PortFlag >	getFlags() CONCT_OVERRIDE_FINAL;

	private:

		uintreg						m_socket;
		sockaddr_in6				m_serverAddress;
		bool						m_connectionReset;
		bool						m_connectionLost;

		Vector< uint8 >				m_sendData;
		Vector< uint8 >				m_receiveData;
	};
}