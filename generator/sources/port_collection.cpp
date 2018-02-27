#include "port_collection.h"

#include "port.h"

#include <filesystem>
#include <iostream>

namespace std
{
	namespace filesystem = std::experimental::filesystem;
}

namespace conct
{
	PortCollection::PortCollection()
	{
	}

	PortCollection::~PortCollection()
	{
		while( !m_ports.empty() )
		{
			delete( m_ports.back() );
			m_ports.pop_back();
		}
	}

	bool PortCollection::load( const std::string& path )
	{
		std::filesystem::directory_iterator iterator( path );
		for( const std::filesystem::directory_entry& entry : iterator )
		{
			Port* pPort = new Port();
			m_ports.push_back( pPort );

			if( !pPort->load( entry.path().generic_string() ) )
			{
				std::cout << "Error: Failed to load hardware from: " << entry.path().generic_string() << std::endl;
				return false;
			}
		}

		return true;
	}

	const Port* PortCollection::findPortByName( const std::string& name ) const
	{
		for( const Port* pPort : m_ports )
		{
			if( pPort->getName() == name )
			{
				return pPort;
			}
		}

		return nullptr;
	}
}
