#pragma once

#include "conct_dynamic_string.h"
#include "conct_vector.h"

namespace conct
{
	class ControllerConfig
	{
	public:

		struct Connection
		{
			DynamicString			title;
			DynamicString			hostname;
			uint16					port;
		};

		struct DeviceFavorite
		{
			uint32					serialNumber;
		};

		struct InstanceFavorite
		{
			uint32					deviceSerialNumber;
			uint16					typeCrc;
		};

		using ConnectionVector = Vector< Connection >;

		void						load();
		void						save();

		const ConnectionVector&		getConnections() const { return m_connections; }
		
		void						addConnection( const Connection& connection );

	private:

		ConnectionVector			m_connections;
	};
}