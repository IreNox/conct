#pragma once

#include "base.h"

namespace conct
{
	class ServiceCollection;

	class Service
	{
		friend class ServiceCollection;

	public:

		const std::string&	getName() const { return m_name;  }
		const std::string&	getTag() const { return m_tag; }

	private: // friend

							Service();
		void				create( const std::string& name, const std::string& tag );

	private:

		std::string			m_name;
		std::string			m_tag;
	};
}