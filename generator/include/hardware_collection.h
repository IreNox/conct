#pragma once

#include "base.h"

namespace conct
{
	class Hardware;

	class HardwareCollection
	{
	public:

									HardwareCollection();
									~HardwareCollection();

		bool						load( const std::string& path );

		const Hardware*				findHardwareByName( const std::string& name ) const;

	private:

		std::vector< Hardware* >	m_hardware;
	};
}