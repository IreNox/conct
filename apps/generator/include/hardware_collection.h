#pragma once

#include <tiki/tiki_dynamic_array.h>

namespace tiki
{
	class DynamicString;
	class Path;
}

namespace conct
{
	class Hardware;

	class HardwareCollection
	{
	public:

							HardwareCollection();
							~HardwareCollection();

		bool				load( const Path& path );

		const Hardware*		findHardwareByName( const DynamicString& name ) const;

	private:

		using HardwareArray = DynamicArray< Hardware* >;

		HardwareArray		m_hardware;
	};
}