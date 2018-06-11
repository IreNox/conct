#pragma once

#include "conct_dynamic_string.h"

namespace conct
{
	class Path;
	class PortCollection;

	class Port
	{
		friend class PortCollection;

	public:

		const DynamicString&	getName() const { return m_name;  }

		const DynamicString&	getLibrary() const { return m_library; }
		const DynamicString&	getClass() const { return m_class; }

		size_t					getMtu() const { return m_mtu; }
		bool					getSingleConnection() const { return m_singleConnection; }
		bool					getMultiConnection() const { return m_multiConnection; }
		bool					getReliable() const { return m_reliable; }

	private: // friend

								Port();

		bool					load( const Path& filename );

	private:

		DynamicString			m_name;

		DynamicString			m_library;
		DynamicString			m_class;

		size_t					m_mtu;
		bool					m_singleConnection;
		bool					m_multiConnection;
		bool					m_reliable;
	};
}