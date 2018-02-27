#pragma once

#include "base.h"

namespace conct
{
	class PortCollection;

	class Port
	{
		friend class PortCollection;

	public:

		const std::string&	getName() const { return m_name;  }

		size_t				getMtu() const { return m_mtu; }
		bool				getSingleConnection() const { return m_singleConnection; }
		bool				getMultiConnection() const { return m_multiConnection; }
		bool				getReliable() const { return m_reliable; }

	private: // friend

							Port();

		bool				load( const std::string& filename );

	private:

		std::string			m_name;

		size_t				m_mtu;
		bool				m_singleConnection;
		bool				m_multiConnection;
		bool				m_reliable;
	};
}