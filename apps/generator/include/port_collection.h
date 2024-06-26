#pragma once

#include <tiki/tiki_dynamic_array.h>

namespace tiki
{
	class DynamicString;
	class Path;
}

namespace conct
{
	class Port;

	class PortCollection
	{
	public:

						PortCollection();
						~PortCollection();

		bool			load( const Path& path );

		const Port*		findPortByName( const DynamicString& name ) const;

	private:

		using PortArray = DynamicArray< Port* >;

		PortArray		m_ports;
	};
}