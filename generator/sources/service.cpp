#include "service.h"

namespace conct
{
	Service::Service()
	{
	}

	void Service::create( const std::string& name, const std::string& tag )
	{
		m_name	= name;
		m_tag	= tag;
	}
}
