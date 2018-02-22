#include "generator.h"

#include "device.h"

#include <filesystem>
#include <iostream>

namespace std
{
	namespace filesystem = std::experimental::filesystem;
}

namespace conct
{
	struct GeneratorParameters
	{
		std::string		deviceFileName;
	};

	Generator::Generator()
	{
	}

	bool Generator::run( const std::vector< std::string >& arguments )
	{
		registerServices();
		registerTypes();

		std::filesystem::path exePath = getExecutableName();
		std::string hardwarePath = exePath.parent_path().parent_path().parent_path().parent_path().append( "hardware" ).generic_string();
		if( !m_hardware.load( hardwarePath, m_services ) )
		{
			return false;
		}

		GeneratorParameters parameters;
		if( !parseArguments( parameters, arguments ) )
		{
			return false;
		}

		Device device;
		if( !device.load( parameters.deviceFileName, m_hardware, m_services, m_types ) )
		{
			std::cout << "Error: Failed to load device from: " << parameters.deviceFileName << std::endl;
			return false;
		}

		return true;
	}

	void Generator::registerServices()
	{
		m_services.addService( "Slave", "slave" );
		m_services.addService( "Host", "host" );
		m_services.addService( "Router", "router" );
	}

	void Generator::registerTypes()
	{
		m_types.addValueType( "", "Boolean" );
		m_types.addValueType( "", "Integer" );
		m_types.addValueType( "", "Unsigned" );
		m_types.addValueType( "", "String" );
		m_types.addValueType( "", "PercentValue" );
	}

	bool Generator::parseArguments( GeneratorParameters& parameters, const std::vector< std::string >& arguments )
	{
		if( arguments.size() < 2u )
		{
			std::cout << "Usage: " << arguments[ 0u ] << " INPUT_FILE" << std::endl;
			return false;
		}

		parameters.deviceFileName = arguments[ 1u ];

		return true;
	}
}
