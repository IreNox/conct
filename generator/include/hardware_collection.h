#pragma once

#include "base.h"
#include "hardware.h"

namespace conct
{
	class HardwareCollection
	{
	public:

									HardwareCollection();
									~HardwareCollection();

		bool						load( const std::string& path, ServiceCollection& serviceCollection );

		const Hardware*				findHardwareByName( const std::string& name ) const;

	private:

		std::vector< Hardware* >	m_hardware;
	};
}