#include "hardware_collection.h"

#include "hardware.h"

#include "conct_directory_iterator.h"
#include "conct_trace.h"

namespace conct
{
	HardwareCollection::HardwareCollection()
	{
	}

	HardwareCollection::~HardwareCollection()
	{
		while( !m_hardware.isEmpty() )
		{
			delete( m_hardware.getBack() );
			m_hardware.popBack();
		}
	}

	bool HardwareCollection::load( const Path& path )
	{
		DirectoryIterator iterator( path );
		while( iterator.next() )
		{
			Hardware* pHardware = new Hardware();
			m_hardware.pushBack( pHardware );

			if( !pHardware->load( iterator.getCurrent() ) )
			{
				trace::write( "Error: Failed to load hardware from: "_s + iterator.getCurrent().getGenericPath() + "\n" );
				return false;
			}
		}

		return true;
	}

	const Hardware* HardwareCollection::findHardwareByName( const DynamicString& name ) const
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
