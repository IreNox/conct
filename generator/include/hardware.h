#pragma once

#include "base.h"

namespace conct
{
	class HardwareCollection;
	class Service;
	class ServiceCollection;

	class Hardware
	{
		friend class HardwareCollection;

	public:

		typedef std::vector< const Service* > ServiceVector;

		const std::string&		getFileName() const { return m_fileName; }
		const std::string&		getName() const { return m_name; }

		const size_t			getProgramDataSize() const { return m_programDataSize; }
		const size_t			getRamSize() const { return m_ramSize; }
		const size_t			getFlashSize() const { return m_flashSize; }

		const ServiceVector&	getServices() const { return m_services;  }

	private: // friend

							Hardware();
		bool				load( const std::string& fileName, ServiceCollection& serviceCollection );

	private:

		std::string			m_fileName;
		std::string			m_name;

		size_t				m_programDataSize;
		size_t				m_ramSize;
		size_t				m_flashSize;

		ServiceVector		m_services;
	};
}