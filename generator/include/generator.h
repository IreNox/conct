#pragma once

#include "base.h"
#include "hardware_collection.h"
#include "service_collection.h"
#include "type_collection.h"

namespace conct
{
	struct GeneratorParameters;

	class Generator
	{
	public:

				Generator();

		bool	run( const std::vector< std::string >& arguments );

	private:

		HardwareCollection	m_hardware;
		ServiceCollection	m_services;
		TypeCollection		m_types;

		void				registerServices();
		void				registerTypes();

		bool				parseArguments( GeneratorParameters& parameters, const std::vector< std::string >& arguments );
	};
}