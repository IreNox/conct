#include "hardware_collection.h"

#include "hardware.h"

#include <filesystem>
#include <iostream>

namespace std
{
	namespace filesystem = std::experimental::filesystem;
}

namespace conct
{
	HardwareCollection::HardwareCollection()
	{
	}

	HardwareCollection::~HardwareCollection()
	{
		while( !m_hardware.empty() )
		{
			delete( m_hardware.back() );
			m_hardware.pop_back();
		}
	}

	bool HardwareCollection::load( const std::string& path )
	{
		std::filesystem::directory_iterator iterator( path );
		for( const std::filesystem::directory_entry& entry : iterator )
		{
			Hardware* pHardware = new Hardware();
			m_hardware.push_back( pHardware );

			if( !pHardware->load( entry.path().generic_string() ) )
			{
				std::cout << "Error: Failed to load hardware from: " << entry.path().generic_string() << std::endl;
				return false;
			}
		}

		return true;
	}

	const Hardware* HardwareCollection::findHardwareByName( const std::string& name ) const
	{
		for( const Hardware* pHardware : m_hardware )
		{
			if( pHardware->getName() == name )
			{
				return pHardware;
			}
		}

		return nullptr;
	}
}
