#include "service_collection.h"

namespace conct
{
	ServiceCollection::ServiceCollection()
	{
	}

	ServiceCollection::~ServiceCollection()
	{
		while( !m_services.empty() )
		{
			delete( m_services.back() );
			m_services.pop_back();
		}
	}

	const Service* ServiceCollection::addService( const std::string& name, const std::string& tag )
	{
		Service* pService = new Service();
		pService->create( name, tag );

		m_services.push_back( pService );
		return pService;
	}

	const Service* ServiceCollection::findServiceByName( const std::string& name ) const
	{
		for( const Service* pService : m_services )
		{
			if( pService->getName() == name )
			{
				return pService;
			}
		}

		return nullptr;
	}

	const conct::Service* ServiceCollection::findServiceByTag( const std::string& tag ) const
	{
		for( const Service* pService : m_services )
		{
			if( pService->getTag() == tag )
			{
				return pService;
			}
		}

		return nullptr;
	}
}
