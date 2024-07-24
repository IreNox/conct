#pragma once

#include <tiki/tiki_dynamic_string.h>

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

		using ConnectionArray = DynamicArray< Connection >;
		using DeviceFavoriteArray = DynamicArray< DeviceFavorite >;
		using InstanceFavoriteArray = DynamicArray< InstanceFavorite >;

		void							load();
		void							save();

		uint32							getRevision() const { return m_revision; }

		const ConnectionArray&			getConnections() const { return m_connections; }
		void							addConnection( const Connection& connection );

		const DeviceFavoriteArray&		getDeviceFavorites() const { return m_deviceFavorites; }
		const InstanceFavoriteArray&	getInstanceFavorites() const { return m_instanceFavorites; }

	private:

		uint32							m_revision			= 0u;

		ConnectionArray					m_connections;
		DeviceFavoriteArray				m_deviceFavorites;
		InstanceFavoriteArray			m_instanceFavorites;
	};
}