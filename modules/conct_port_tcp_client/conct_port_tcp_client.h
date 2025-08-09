#pragma once

#include "conct_port.h"

#include <tiki/tiki_dynamic_string.h>

#if TIKI_ENABLED( TIKI_PLATFORM_WINDOWS )
#	include <WinSock2.h>
#	include <ws2ipdef.h>
#elif TIKI_ENABLED( TIKI_PLATFORM_LINUX ) || TIKI_ENABLED( TIKI_PLATFORM_ANDROID )
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

		virtual void				getEndpoints( ArrayView< const uintreg >& endpoints ) TIKI_OVERRIDE_FINAL;
		virtual bool				popConnectionReset( uintreg& endpointId ) TIKI_OVERRIDE_FINAL;

		virtual void				loop() TIKI_OVERRIDE_FINAL;

		virtual bool				openSend( BinaryWriter& writer, uintreg size, uintreg endpointId ) TIKI_OVERRIDE_FINAL;
		virtual void				closeSend( BinaryWriter& writer, uintreg endpointId ) TIKI_OVERRIDE_FINAL;

		virtual bool				openReceived( BinaryReader& reader, uintreg& endpointId ) TIKI_OVERRIDE_FINAL;
		virtual void				closeReceived( BinaryReader& reader, uintreg endpointId ) TIKI_OVERRIDE_FINAL;

		virtual Flags8< PortFlag >	getFlags() const TIKI_OVERRIDE_FINAL;

	private:

		uintreg						m_socket;
		sockaddr_in6				m_serverAddress;
		bool						m_connectionReset;
		bool						m_connectionLost;

		DynamicArray< uint8 >		m_sendData;
		DynamicArray< uint8 >		m_receiveData;
	};
}