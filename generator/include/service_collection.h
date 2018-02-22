#pragma once

#include "service.h"

namespace conct
{
	class ServiceCollection
	{
	public:

								ServiceCollection();
								~ServiceCollection();

		const Service*			addService( const std::string& name, const std::string& tag );
		const Service*			findServiceByName( const std::string& name ) const;
		const Service*			findServiceByTag( const std::string& tag ) const;

	private:

		std::vector< Service* >	m_services;
	};
}