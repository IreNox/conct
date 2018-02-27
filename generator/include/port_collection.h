#pragma once

#include <string>
#include <vector>

namespace conct
{
	class Port;

	class PortCollection
	{
	public:

								PortCollection();
								~PortCollection();

		bool					load( const std::string& path );

		const Port*				findPortByName( const std::string& name ) const;

	private:

		std::vector< Port* >	m_ports;
	};
}