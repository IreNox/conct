#pragma once

#include "conct_vector.h"

namespace conct
{
	class DynamicString;
	class Hardware;
	class Path;

	class HardwareCollection
	{
	public:

							HardwareCollection();
							~HardwareCollection();

		bool				load( const Path& path );

		const Hardware*		findHardwareByName( const DynamicString& name ) const;

	private:

		Vector< Hardware* >	m_hardware;
	};
}