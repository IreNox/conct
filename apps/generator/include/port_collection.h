#pragma once

#include "conct_vector.h"

namespace conct
{
	class DynamicString;
	class Path;
	class Port;

	class PortCollection
	{
	public:

								PortCollection();
								~PortCollection();

		bool					load( const Path& path );

		const Port*				findPortByName( const DynamicString& name ) const;

	private:

		Vector< Port* >	m_ports;
	};
}