#pragma once

#include "conct_port.h"
#include "i_simulator_port.h"

#include <mutex>
#include <queue>
#include <vector>

namespace conct
{
	class PortRs485Sim : public Port, private ISimulatorPort
	{
	public:

		virtual void				setup() CONCT_OVERRIDE_FINAL;
		virtual void				loop() CONCT_OVERRIDE_FINAL;

		virtual bool				openSend( Writer& writer, uintreg size, DeviceId deviceId ) CONCT_OVERRIDE_FINAL;
		virtual void				closeSend( Writer& writer ) CONCT_OVERRIDE_FINAL;

		virtual bool				openReceived( Reader& reader, DeviceId& deviceId ) CONCT_OVERRIDE_FINAL;
		virtual void				closeReceived( Reader& reader ) CONCT_OVERRIDE_FINAL;

		virtual Flags8< PortFlag >	getFlags() CONCT_OVERRIDE_FINAL;

	private:

		enum
		{
			MaxPacketSize = 24u
		};

		typedef std::vector< uint8 > Packet;
		typedef std::queue< Packet > PacketQueue;

		ISimulatorPort*				m_pCounterpartPort;

		std::mutex					m_receiveMutex;
		PacketQueue					m_receivedPackets;
		uint8						m_receiveBuffer[ MaxPacketSize ];
		uint8						m_sendBuffer[ MaxPacketSize ];

		virtual void				setCounterpartPort( ISimulatorPort* pPort ) CONCT_OVERRIDE_FINAL;

		virtual void				pushData( const void* pData, uintreg dataSize ) CONCT_OVERRIDE_FINAL;
	};
}