#include "conct_port_rs485_sim.h"

#include "conct_functions.h"
#include "conct_memory.h"
#include "conct_reader.h"
#include "conct_writer.h"

#define CONCT_IS_DLL CONCT_ON
#include "i_simulator_context.h"

namespace conct
{
	void PortRs485Sim::setup()
	{
		m_pCounterpartPort = nullptr;

		getSimulatorContext().registerPort( this );
	}

	void PortRs485Sim::loop()
	{
	}

	bool PortRs485Sim::openSend( Writer& writer, uintreg size, uintreg endpointId )
	{
		if( m_pCounterpartPort == nullptr )
		{
			return false;
		}

		writer.set( m_sendBuffer, sizeof( m_sendBuffer ) );
		return size > 0u;
	}

	void PortRs485Sim::closeSend( Writer& writer, uintreg endpointId )
	{
		const uintreg writtenBytes = sizeof( m_sendBuffer ) - writer.getRemainingSize();
		if( writtenBytes == 0u )
		{
			return;
		}

		m_pCounterpartPort->pushData( m_sendBuffer, writtenBytes );
	}

	bool PortRs485Sim::openReceived( Reader& reader, uintreg& endpointId )
	{
		std::lock_guard< std::mutex > lock( m_receiveMutex );

		if( m_receivedPackets.empty() )
		{
			return false;
		}

		const Packet& packet = m_receivedPackets.front();
		const uintreg dataSize = CONCT_MIN( sizeof( m_receiveBuffer ), packet.size() );
		memory::copy( m_receiveBuffer, packet.data(), dataSize );
		m_receivedPackets.pop();

		reader.set( m_receiveBuffer, dataSize );
		endpointId = 0u;
		return true;
	}

	void PortRs485Sim::closeReceived( Reader& reader, uintreg endpointId )
	{
	}

	Flags8< PortFlag > PortRs485Sim::getFlags()
	{
		Flags8< PortFlag > flags;
		flags |= PortFlag_SingleEndpoint;
		flags |= PortFlag_Reliable;
		return flags;
	}

	void PortRs485Sim::setCounterpartPort( ISimulatorPort* pPort )
	{
		m_pCounterpartPort = pPort;
	}

	void PortRs485Sim::pushData( const void* pData, uintreg dataSize )
	{
		std::lock_guard< std::mutex > lock( m_receiveMutex );

		m_receivedPackets.push( Packet( dataSize ) );
		Packet& packet = m_receivedPackets.back();
		memory::copy( packet.data(), pData, dataSize );
	}
}
