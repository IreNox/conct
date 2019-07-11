#pragma once

#include "conct_map.h"
#include "conct_mutex.h"
#include "conct_queue.h"

namespace conct
{
	class SimulatorRf24Context
	{
	public:

		bool					available( uint64 address ) const;

		void					read( uint64 address, void* pBuffer, uint8 bufferLength );
		void					write( uint64 address, const void* pData, uint8 dataLength );

	private:

		static const uint8 MaxPackageSize = 32u;

		struct Package
		{
			uint8				data[ MaxPackageSize ];
			uint8				size;
		};

		struct Endpoint
		{
			Queue< Package >	packages;
		};

		using EndpointMap = Map< uint64, Endpoint >;

		Mutex					m_mutex;
		EndpointMap				m_endpoints;
	};
}