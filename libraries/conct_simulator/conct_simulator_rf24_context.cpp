#include "conct_simulator_rf24_context.h"

#include "conct_memory.h"

namespace conct
{
	bool SimulatorRf24Context::available( uint64 address ) const
	{
		MutexLock lock( m_mutex );

		const Endpoint* pEndpoint = m_endpoints.find( address );
		if( pEndpoint == nullptr )
		{
			return false;
		}

		return !pEndpoint->packages.isEmpty();
	}

	void SimulatorRf24Context::read( uint64 address, void* pBuffer, uint8 bufferLength )
	{
		MutexLock lock( m_mutex );

		Endpoint* pEndpoint = m_endpoints.find( address );
		if( pEndpoint == nullptr )
		{
			return;
		}

		const Package& package = pEndpoint->packages.getFront();
		memory::copy( pBuffer, package.data, CONCT_MIN( package.size, bufferLength ) );
		pEndpoint->packages.popFront();
	}

	void SimulatorRf24Context::write( uint64 address, const void* pData, uint8 dataLength )
	{
		MutexLock lock( m_mutex );

		EndpointMap::InsertResult insertResult = m_endpoints.insertKey( address );
		Endpoint* pEndpoint = insertResult.pValue;

		Package& package = pEndpoint->packages.pushBack();
		memory::copy( package.data, pData, CONCT_MIN( sizeof( package.data ), dataLength ) );
		package.size = dataLength;
	}
}
