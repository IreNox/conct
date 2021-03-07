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
		using DeviceFavoriteVector = Vector< DeviceFavorite >;
		using InstanceFavoriteVector = Vector< InstanceFavorite >;

		void							load();
		void							save();

		uint32							getRevision() const { return m_revision; }

		const ConnectionVector&			getConnections() const { return m_connections; }
		void							addConnection( const Connection& connection );

		const DeviceFavoriteVector&		getDeviceFavorites() const { return m_deviceFavorites; }
		const InstanceFavoriteVector&	getInstanceFavorites() const { return m_instanceFavorites; }
		
	private:

		uint32							m_revision			= 0u;

		ConnectionVector				m_connections;
		DeviceFavoriteVector			m_deviceFavorites;
		InstanceFavoriteVector			m_instanceFavorites;
	};
}