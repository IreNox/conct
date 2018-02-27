#include "generator.h"

#include "device.h"
#include "interface.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace std
{
	namespace filesystem = std::experimental::filesystem;
}

namespace conct
{
	struct GeneratorParameters
	{
		std::string		deviceFileName;
		std::string		outputPath;
	};

	Generator::Generator()
	{
	}

	bool Generator::run( const std::vector< std::string >& arguments )
	{
		registerTypes();

		std::filesystem::path exePath = getExecutableName();
		std::filesystem::path basePath = exePath.parent_path().parent_path().parent_path().parent_path();
		std::filesystem::path configPath = basePath;
		configPath.append( "config" );

		std::filesystem::path hardwarePath = configPath;
		hardwarePath.append( "hardware" );
		if( !m_hardware.load( hardwarePath.generic_string() ) )
		{
			return false;
		}

		std::filesystem::path portsPath = configPath;
		portsPath.append( "ports" );
		if( !m_ports.load( portsPath.generic_string() ) )
		{
			return false;
		}

		std::filesystem::path typesPath = configPath;
		typesPath.append( "types" );
		if( !m_types.load( typesPath.generic_string() ) )
		{
			return false;
		}

		GeneratorParameters parameters;
		if( !parseArguments( parameters, arguments ) )
		{
			return false;
		}

		if( !std::filesystem::exists( parameters.outputPath ) &&
			!std::filesystem::create_directory( parameters.outputPath ) )
		{
			std::cout << "Error: Failed to create output directory: " << parameters.outputPath << std::endl;
			return false;
		}

		Device device;
		if( !device.load( parameters.deviceFileName, m_hardware, m_ports, m_types ) )
		{
			std::cout << "Error: Failed to load device from: " << parameters.deviceFileName << std::endl;
			return false;
		}

		if( !exportInterfaces( device, parameters ) ||
			!exportDeviceClass( device, parameters ) )
		{
			return false;
		}

		return true;
	}

	void Generator::registerTypes()
	{
		m_types.addValueType( "", "Boolean" );
		m_types.addValueType( "", "Integer" );
		m_types.addValueType( "", "Unsigned" );
		m_types.addValueType( "", "String" );
		m_types.addValueType( "", "PercentValue" );
		m_types.addValueType( "", "Guid" );
		m_types.addValueType( "", "TypeDescription" ); // fake enum
	}

	std::string Generator::combinePath( const std::string& path1, const std::string& path2 )
	{
		if( path1.back() == '/' || path1.back() == '\\' )
		{
			return path1 + path2;
		}

		return path1 + '\\' + path2;
	}

	std::string Generator::replaceString( const std::string& text, const std::string& from, const std::string& to ) const
	{
		size_t startIndex = 0;
		std::string str = text;
		while( ( startIndex = str.find( from, startIndex ) ) != std::string::npos )
		{
			str.replace( startIndex, from.length(), to );
			startIndex += to.length(); // Handles case where 'to' is a substring of 'from'
		}

		return str;
	}

	std::string Generator::getTypeFilename( const Type* pType, const std::string& appendix, const std::string& extension ) const
	{
		std::string filename = pType->getFullName();
		std::replace( filename.begin(), filename.end(), '.', '_' );
		std::transform( filename.begin(), filename.end(), filename.begin(), ::tolower );
		filename += appendix;
		filename += "." + extension;

		return filename;
	}

	std::string Generator::getTypeCppName( const Type* pType ) const
	{
		if( pType == nullptr )
		{
			return "void";
		}

		std::string typeName = pType->getFullName();
		replaceString( typeName, ".", "::" );

		return typeName;
	}

	std::string Generator::getTypeCppReturnName( const Type* pType ) const
	{
		if( pType->getDescription() == TypeDescription_Array )
		{
			return "Array< " + pType->getFullName() + " >";
		}
		else if( pType->getDescription() == TypeDescription_Interface )
		{
			//return
		}

		return getTypeCppName( pType );
	}

	std::string Generator::getTypeCppArgumentName( const Type* pType ) const
	{
		return getTypeCppName( pType );
	}

	bool Generator::writeStringToFile( const std::string& filename, const std::string& content ) const
	{
		std::ofstream fileStream( filename, std::ofstream::out | std::ofstream::trunc );
		if( !fileStream.is_open() )
		{
			std::cout << "Error: Failed to open '" << filename << "'" << std::endl;
			return false;
		}

		fileStream << content;
		fileStream.close();

		return true;
	}

	bool Generator::parseArguments( GeneratorParameters& parameters, const std::vector< std::string >& arguments )
	{
		if( arguments.size() < 2u )
		{
			std::cout << "Usage: " << arguments[ 0u ] << " INPUT_FILE" << std::endl;
			return false;
		}

		for( const std::string& argument : arguments )
		{
			if( argument.empty() || argument == arguments[ 0u ] )
			{
				continue;
			}

			if( argument[ 0u ] == '-' )
			{
				const size_t assignIndex = argument.find_first_of( '=' );
				if( assignIndex == std::string::npos )
				{
					std::cout << "Warning: Invalid parameter '" << argument << "'" << std::endl;
					continue;
				}

				const std::string argumentName = argument.substr( 0u, assignIndex );
				const std::string argumentValue = argument.substr( assignIndex + 1u );
				if( argumentName == "--output" )
				{
					parameters.outputPath = argumentValue;
				}
				else
				{
					std::cout << "Warning: Unknown parameter '" << argumentName << "'" << std::endl;
				}
			}
			else
			{
				if( !parameters.deviceFileName.empty() )
				{
					std::cout << "Warning: Invalid second input file '" << argument << "'" << std::endl;
					continue;
				}

				parameters.deviceFileName = argument;
			}
		}

		if( parameters.deviceFileName.empty() )
		{
			std::cout << "Error: No input file given" << std::endl;
			return false;
		}

		if( parameters.outputPath.empty() )
		{
			std::cout << "Error: No input file given" << std::endl;
			return false;
		}

		return true;
	}

	bool Generator::exportInterfaces( const Device& device, const GeneratorParameters& parameters )
	{
		for( const Interface* pInterface : device.getInterfaces() )
		{
			const std::string headerFilename = getTypeFilename( pInterface, "", "h" );

			std::ostringstream headerContent;
			headerContent << "#pragma once" << std::endl;
			headerContent << std::endl;
			headerContent << "#include \"conct_core.h\"" << std::endl;
			headerContent << std::endl;
			headerContent << "namespace conct" << std::endl;
			headerContent << "{" << std::endl;
			headerContent << "\tclass " << pInterface->getName() << std::endl;
			headerContent << "\t{" << std::endl;
			headerContent << "\tpublic:" << std::endl;
			headerContent << std::endl;

			for( const InterfaceProperty& property : pInterface->getProperties() )
			{
				if( property.hasGetter )
				{
					headerContent << "\t\t" << getTypeCppReturnName( property.pType ) << " get" << property.name << "() const = 0;" << std::endl;
				}

				if( property.hasSetter )
				{
					headerContent << "\t\tvoid set" << property.name << "( " << getTypeCppArgumentName( property.pType ) << " value ) = 0;" << std::endl;
				}
			}

			if( !pInterface->getProperties().empty() && !pInterface->getFunctions().empty() )
			{
				headerContent << std::endl;
			}

			for( const InterfaceFunction& function : pInterface->getFunctions() )
			{
				std::string parameters;
				for( const InterfaceFunctionParameter& parameter : function.parameters )
				{
					if( !parameters.empty() )
					{
						parameters += ", ";
					}

					parameters += getTypeCppArgumentName( parameter.pType ) + " " + parameter.name;
				}

				headerContent << "\t\t" << getTypeCppReturnName( function.pReturnType ) << " " << function.name << "( " << parameters << " ) = 0;" << std::endl;
			}

			headerContent << "\t};" << std::endl;
			headerContent << "}" << std::endl;

			const std::string headerFilepath = combinePath( parameters.outputPath, headerFilename );
			if( !writeStringToFile( headerFilepath, headerContent.str() ) )
			{
				return false;
			}
		}

		return true;
	}

	bool Generator::exportDeviceClass( const Device& device, const GeneratorParameters& parameters )
	{
		std::ostringstream headerContent;
		headerContent << "#pragma once" << std::endl;
		headerContent << std::endl;
		headerContent << "#include \"conct_array.h\"" << std::endl;
		headerContent << "#include \"conct_core.h\"" << std::endl;
		headerContent << std::endl;

		for( const Interface* pInterface : device.getInterfaces() )
		{
			headerContent << "#include \"" << getTypeFilename( pInterface, "", "h" ) << "\"" << std::endl;
		}

		headerContent << std::endl;
		headerContent << "namespace conct" << std::endl;
		headerContent << "{" << std::endl;
		headerContent << "\tstruct ProxyRegistry;" << std::endl;
		headerContent << std::endl;
		headerContent << "\tclass Device" << device.getName() << std::endl;
		headerContent << "\t{" << std::endl;
		headerContent << "\tpublic:" << std::endl;
		headerContent << std::endl;
		headerContent << "\t\tvoid getProxies( Array< ProxyRegistry >& proxies ) const;" << std::endl;
		headerContent << std::endl;
		headerContent << "\tprivate:" << std::endl;
		headerContent << std::endl;

		for( const Interface* pInterface : device.getInterfaces() )
		{
			headerContent << "\t\t" << getTypeCppName( pInterface ) << " m_" << pInterface->getName() << ";" << std::endl;
		}

		headerContent << "\t};" << std::endl;
		headerContent << "}" << std::endl;

		std::ostringstream sourceContent;
		sourceContent << "#include \"device.h\"" << std::endl;
		sourceContent << std::endl;
		sourceContent << "#include \"conct_array.h\"" << std::endl;
		sourceContent << std::endl;
		sourceContent << "namespace conct" << std::endl;
		sourceContent << "{" << std::endl;
		sourceContent << "\tvoid Device" << device.getName() << "::getProxies( Array< ProxyRegistry >& proxies ) const" << std::endl;
		sourceContent << "\t{" << std::endl;
		sourceContent << "\t\tstatic const ProxyRegistry s_proxies[] =" << std::endl;
		sourceContent << "\t\t{" << std::endl;

		for( const Interface* pInterface : device.getInterfaces() )
		{
			sourceContent << "\t\t\t{ \"" << pInterface->getFullName() << "\", &m_" << pInterface->getName() << " }," << std::endl;
		}

		sourceContent << "\t\t};" << std::endl;
		sourceContent << std::endl;
		sourceContent << "\t\tproxies.set( s_proxies, CONCT_COUNT( s_proxies ) );" << std::endl;
		sourceContent << "\t}" << std::endl;
		sourceContent << "}" << std::endl;

		const std::string headerFilepath = combinePath( parameters.outputPath, "device.h" );
		const std::string sourceFilepath = combinePath( parameters.outputPath, "device.cpp" );
		return writeStringToFile( headerFilepath, headerContent.str() ) &&
			writeStringToFile( sourceFilepath, sourceContent.str() );
	}
}
