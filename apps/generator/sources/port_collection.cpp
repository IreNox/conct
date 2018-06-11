#include "port_collection.h"

#include "port.h"

#include "conct_directory_iterator.h"

#include <iostream>

namespace conct
{
	PortCollection::PortCollection()
	{
	}

	PortCollection::~PortCollection()
	{
		while( !m_ports.isEmpty() )
		{
			delete( m_ports.getLast() );
			m_ports.popBack();
		}
	}

	bool PortCollection::load( const Path& path )
	{
		DirectoryIterator iterator( path );
		while( iterator.next() )
		{
			Port* pPort = new Port();
			m_ports.pushBack( pPort );

			if( !pPort->load( iterator.getCurrent() ) )
			{
				std::cout << "Error: Failed to load hardware from: " << iterator.getCurrent().getGenericPath() << std::endl;
				return false;
			}
		}

		return true;
	}

	const Port* PortCollection::findPortByName( const DynamicString& name ) const
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
