#include "generator.h"

#include "device.h"
#include "hardware.h"
#include "interface.h"
#include "port.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace conct
{
	struct GeneratorParameters
	{
		std::filesystem::path	deviceFileName;
		std::filesystem::path	outputPath;
		std::filesystem::path	genOutputPath;
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

		if( !std::filesystem::exists( parameters.genOutputPath ) &&
			!std::filesystem::create_directory( parameters.genOutputPath ) )
		{
			std::cout << "Error: Failed to create gen output directory: " << parameters.genOutputPath << std::endl;
			return false;
		}

		Device device;
		if( !device.load( parameters.deviceFileName.generic_string(), m_hardware, m_ports, m_types ) )
		{
			std::cout << "Error: Failed to load device from: " << parameters.deviceFileName << std::endl;
			return false;
		}

		if( !exportInterfaces( device, parameters ) ||
			!exportInterfaceProxies( device, parameters ) ||
			!exportInterfaceImpl( device, parameters ) ||
			!exportDeviceInterface( device, parameters ) ||
			!exportDeviceImpl( device, parameters ) ||
			!exportIno( device, parameters ) )
		{
			return false;
		}

		return true;
	}

	void Generator::registerTypes()
	{
		m_types.addValueType( "", "Boolean", "bool" );
		m_types.addValueType( "", "Integer", "int" );
		m_types.addValueType( "", "Unsigned", "unsigned" );
		m_types.addValueType( "", "String", "std::string" );
		m_types.addValueType( "", "PercentValue", "PercentValue" );
		m_types.addValueType( "", "Guid", "Guid" );
		m_types.addValueType( "", "TypeDescription", "error" ); // fake enum
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

		if( !appendix.empty() )
		{
			filename += "_" + appendix;
		}

		if( !extension.empty() )
		{
			filename += "." + extension;
		}

		return filename;
	}

	std::string Generator::getTypeCppName( const Type* pType ) const
	{
		if( pType == nullptr )
		{
			return "void";
		}

		//std::string typeNamespace = pType->getNamespace();
		//replaceString( typeNamespace, ".", "::" );

		std::string typeName;
		//if( typeNamespace.empty() )
		{
			typeName = pType->getCppName();
		}
		//else
		//{
		//	typeName = typeNamespace + "::" + pType->getCppName();
		//}

		return typeName;
	}

	std::string Generator::getTypeCppReturnName( const Type* pType ) const
	{
		//if( pType->getDescription() == TypeDescription_Array )
		//{
		//	return "Array< " + pType->getFullName() + " >";
		//}
		//else if( pType->getDescription() == TypeDescription_Interface )
		//{
		//	//return
		//}

		return getTypeCppName( pType );
	}

	std::string Generator::getTypeCppArgumentName( const Type* pType ) const
	{
		return getTypeCppName( pType );
	}

	std::string Generator::getDeviceFilename( const Device& device ) const
	{
		std::string filename;

		for( char c : device.getName() )
		{
			if( std::isupper( c ) )
			{
				if( !filename.empty() )
				{
					filename.push_back( '_' );
				}

				filename.push_back( std::tolower( c ) );
			}
			else
			{
				filename.push_back( c );
			}
		}

		return filename;
	}

	bool Generator::writeStringToFile( const std::filesystem::path& filename, const std::string& content, bool ifNotExists )
	{
		m_exportedFiles.insert( filename );

		{
			std::ifstream readStream( filename );
			if( readStream.is_open() )
			{
				if( ifNotExists )
				{
					return true;
				}

				std::string currentContent( ( std::istreambuf_iterator<char>( readStream ) ), std::istreambuf_iterator<char>() );
				if( currentContent == content )
				{
					return true;
				}
			}

			readStream.close();
		}

		std::ofstream writeStream( filename, std::ofstream::out | std::ofstream::trunc );
		if( !writeStream.is_open() )
		{
			std::cout << "Error: Failed to open '" << filename << "'" << std::endl;
			return false;
		}

		writeStream << content;
		writeStream.close();

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
			parameters.outputPath = parameters.deviceFileName.parent_path();
		}

		if( parameters.genOutputPath.empty() )
		{
			parameters.genOutputPath = parameters.outputPath;
			parameters.genOutputPath.append( "gen" );
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
			headerContent << "\t\tstatic const TypeCrc s_TypeCrc = " << pInterface->getCrc() << ";" << std::endl;
			headerContent << std::endl;

			for( const InterfaceProperty& property : pInterface->getProperties() )
			{
				if( property.hasGetter )
				{
					headerContent << "\t\tvirtual " << getTypeCppReturnName( property.pType ) << " get" << property.name << "() const = 0;" << std::endl;
				}

				if( property.hasSetter )
				{
					headerContent << "\t\tvirtual void set" << property.name << "( " << getTypeCppArgumentName( property.pType ) << " value ) = 0;" << std::endl;
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

				headerContent << "\t\tvirtual " << getTypeCppReturnName( function.pReturnType ) << " " << function.name << "( " << parameters << " ) = 0;" << std::endl;
			}

			headerContent << "\t};" << std::endl;
			headerContent << "}" << std::endl;

			const std::filesystem::path headerFilepath = parameters.genOutputPath / headerFilename;
			if( !writeStringToFile( headerFilepath, headerContent.str(), false ) )
			{
				return false;
			}
		}

		return true;
	}

	bool Generator::exportInterfaceProxies( const Device& device, const GeneratorParameters& parameters )
	{
		for( const Interface* pInterface : device.getProxies() )
		{
			const std::string interfaceHeaderFilename = getTypeFilename( pInterface, "", "h" );

			const std::string headerFilename = getTypeFilename( pInterface, "proxy", "h" );
			const std::string sourceFilename = getTypeFilename( pInterface, "proxy", "cpp" );

			const std::string proxyName = pInterface->getName() + "Proxy";

			std::ostringstream headerContent;
			headerContent << "#pragma once" << std::endl;
			headerContent << std::endl;
			headerContent << "#include \"conct_proxy.h\"" << std::endl;
			headerContent << std::endl;
			headerContent << "namespace conct" << std::endl;
			headerContent << "{" << std::endl;
			headerContent << "\tclass " << proxyName << " : public Proxy" << std::endl;
			headerContent << "\t{" << std::endl;
			headerContent << "\tpublic:" << std::endl;
			headerContent << std::endl;
			headerContent << "\t\t" << proxyName << "();" << std::endl;
			headerContent << std::endl;
			headerContent << "\t\tvirtual bool getProperty( Value& targetValue, const void* pInstance, const char* pName ) const CONCT_OVERRIDE_FINAL;" << std::endl;
			headerContent << "\t\tvirtual bool setProperty( void* pInstance, const char* pName, const Value& value ) CONCT_OVERRIDE_FINAL;" << std::endl;
			headerContent << std::endl;
			headerContent << "\t\tvirtual bool callFunction( Value& returnValue, void* pInstance, const char* pName, const Array< Value >& parameters ) CONCT_OVERRIDE_FINAL;" << std::endl;
			headerContent << "\t};" << std::endl;
			headerContent << "}" << std::endl;

			std::ostringstream sourceContent;
			sourceContent << "#include \"" << headerFilename << "\"" << std::endl;
			sourceContent << std::endl;
			sourceContent << "#include \"" << interfaceHeaderFilename << "\"" << std::endl;
			sourceContent << std::endl;
			sourceContent << "namespace conct" << std::endl;
			sourceContent << "{" << std::endl;

			sourceContent << "\t" << proxyName << "::" << proxyName << "()" << std::endl;
			sourceContent << "\t\t: Proxy( 0u )" << std::endl; // todo: type crc
			sourceContent << "\t{" << std::endl;
			sourceContent << "\t}" << std::endl;
			sourceContent << std::endl;

			sourceContent << "\tbool " << proxyName << "::getProperty( Value& targetValue, const void* pInstance, const char* pName ) const" << std::endl;
			sourceContent << "\t{" << std::endl;
			sourceContent << "\t\tconst " << pInterface->getName() << "* pTypedInstance = static_cast< const " << pInterface->getName() << "* >( pInstance );" << std::endl;
			sourceContent << std::endl;

			for( const InterfaceProperty& property : pInterface->getProperties() )
			{
				if( !property.hasGetter )
				{
					continue;
				}

				sourceContent << "\t\tif( isStringEqual( pName, \"" << property.name << "\" ) )" << std::endl;
				sourceContent << "\t\t{" << std::endl;
				sourceContent << "\t\t\ttargetValue.set" << property.pType->getName() << "( pTypedInstance->get" << property.name << "() );" << std::endl;
				sourceContent << "\t\t\treturn true;" << std::endl;
				sourceContent << "\t\t}" << std::endl;
			}

			sourceContent << std::endl;
			sourceContent << "\t\treturn false;" << std::endl;
			sourceContent << "\t}" << std::endl;
			sourceContent << std::endl;

			sourceContent << "\tbool " << proxyName << "::setProperty( void* pInstance, const char* pName, const Value& value )" << std::endl;
			sourceContent << "\t{" << std::endl;
			sourceContent << "\t\t" << pInterface->getName() << "* pTypedInstance = static_cast< " << pInterface->getName() << "* >( pInstance );" << std::endl;
			sourceContent << std::endl;

			for( const InterfaceProperty& property : pInterface->getProperties() )
			{
				if( !property.hasSetter )
				{
					continue;
				}

				sourceContent << "\t\tif( isStringEqual( pName, \"" << property.name << "\" ) )" << std::endl;
				sourceContent << "\t\t{" << std::endl;
				sourceContent << "\t\t\tpTypedInstance->set" << property.name << "( value.get" <<  property.pType->getName() << "() );" << std::endl;
				sourceContent << "\t\t\treturn true;" << std::endl;
				sourceContent << "\t\t}" << std::endl;
			}

			sourceContent << std::endl;
			sourceContent << "\t\treturn false;" << std::endl;
			sourceContent << "\t}" << std::endl;
			sourceContent << std::endl;

			sourceContent << "\tbool " << pInterface->getName() << "Proxy::callFunction( Value& returnValue, void* pInstance, const char* pName, const Array< Value >& parameters )" << std::endl;
			sourceContent << "\t{" << std::endl;
			sourceContent << "\t\tconst " << pInterface->getName() << "* pTypedInstance = static_cast< " << pInterface->getName() << "* >( pInstance );" << std::endl;
			sourceContent << std::endl;

			for( const InterfaceFunction& function : pInterface->getFunctions() )
			{
				sourceContent << "\t\tif( isStringEqual( pName, \"" << function.name << "\" ) )" << std::endl;
				sourceContent << "\t\t{" << std::endl;
				sourceContent << "\t\t\tpTypedInstance->" << function.name << "( value.get" << function.pReturnType->getName() << "() );" << std::endl;
				sourceContent << "\t\t\treturn true;" << std::endl;
				sourceContent << "\t\t}" << std::endl;
			}

			sourceContent << std::endl;
			sourceContent << "\t\treturn false;" << std::endl;
			sourceContent << "\t}" << std::endl;

			sourceContent << "}" << std::endl;

			const std::filesystem::path headerFilepath = parameters.genOutputPath / headerFilename;
			const std::filesystem::path sourceFilepath = parameters.genOutputPath / sourceFilename;
			if( !writeStringToFile( headerFilepath, headerContent.str(), false ) ||
				!writeStringToFile( sourceFilepath, sourceContent.str(), false ) )
			{
				return false;
			}
		}

		return true;
	}

	bool Generator::exportInterfaceImpl( const Device& device, const GeneratorParameters& parameters )
	{
		for( const DeviceInstance& instance : device.getInstances() )
		{
			const Interface* pInterface = instance.pInterface;

			const std::string interfaceHeaderFilename = getTypeFilename( pInterface, "", "h" );

			const std::string headerFilename = getTypeFilename( pInterface, "impl", "h" );
			const std::string sourceFilename = getTypeFilename( pInterface, "impl", "cpp" );

			const std::string className = pInterface->getName() + "Impl";

			std::ostringstream headerContent;
			headerContent << "#pragma once" << std::endl;
			headerContent << std::endl;
			headerContent << "#include \"gen/" << interfaceHeaderFilename << "\"" << std::endl;
			headerContent << std::endl;
			headerContent << "namespace conct" << std::endl;
			headerContent << "{" << std::endl;
			headerContent << "\tclass " << className << " : " << pInterface->getName() << std::endl;
			headerContent << "\t{" << std::endl;
			headerContent << "\tpublic:" << std::endl;
			headerContent << std::endl;

			for( const InterfaceProperty& property : pInterface->getProperties() )
			{
				if( property.hasGetter )
				{
					headerContent << "\t\tvirtual " << getTypeCppReturnName( property.pType ) << " get" << property.name << "() const CONCT_OVERRIDE_FINAL;" << std::endl;
				}

				if( property.hasSetter )
				{
					headerContent << "\t\tvirtual void set" << property.name << "( " << getTypeCppArgumentName( property.pType ) << " value ) CONCT_OVERRIDE_FINAL;" << std::endl;
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

				headerContent << "\t\tvirtual " << getTypeCppReturnName( function.pReturnType ) << " " << function.name << "( " << parameters << " ) CONCT_OVERRIDE_FINAL;" << std::endl;
			}

			headerContent << "\t};" << std::endl;
			headerContent << "}" << std::endl;

			std::ostringstream sourceContent;
			sourceContent << "#include \"" << headerFilename << "\"" << std::endl;
			sourceContent << std::endl;
			sourceContent << "namespace conct" << std::endl;
			sourceContent << "{" << std::endl;

			for( const InterfaceProperty& property : pInterface->getProperties() )
			{
				if( property.hasGetter )
				{
					sourceContent << "\t" << getTypeCppReturnName( property.pType ) << " " << className << "::get" << property.name << "() const" << std::endl;
					sourceContent << "\t{" << std::endl;
					sourceContent << "\t\t// TODO: implement" << std::endl;
					sourceContent << "\t}" << std::endl;
					sourceContent << std::endl;
				}

				if( property.hasSetter )
				{
					sourceContent << "\tvoid " << className << "::set" << property.name << "( " << getTypeCppArgumentName( property.pType ) << " value )" << std::endl;
					sourceContent << "\t{" << std::endl;
					sourceContent << "\t\t// TODO: implement" << std::endl;
					sourceContent << "\t}" << std::endl;
					sourceContent << std::endl;
				}
			}

			if( !pInterface->getProperties().empty() && !pInterface->getFunctions().empty() )
			{
				sourceContent << std::endl;
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

				sourceContent << "\t" << getTypeCppReturnName( function.pReturnType ) << " " << className << "::" << function.name << "( " << parameters << " )" << std::endl;
				sourceContent << "\t{" << std::endl;
				sourceContent << "\t\t// TODO: implement" << std::endl;
				sourceContent << "\t}" << std::endl;
				sourceContent << std::endl;
			}

			sourceContent << "}" << std::endl;

			const std::filesystem::path headerFilepath = parameters.outputPath / headerFilename;
			const std::filesystem::path sourceFilepath = parameters.outputPath / sourceFilename;
			if( !writeStringToFile( headerFilepath, headerContent.str(), true ) ||
				!writeStringToFile( sourceFilepath, sourceContent.str(), true ) )
			{
				return false;
			}
		}

		return true;
	}

	bool Generator::exportDeviceInterface( const Device& device, const GeneratorParameters& parameters )
	{
		const HardwareRuntime runtime = device.getHardware().getRuntime();
		const std::string runitmeHeaderFilename = (runtime == HardwareRuntime_Low ? "conct_runtime_low.h" : "conct_runtime_high.h");
		const std::string runitmeClassName = ( runtime == HardwareRuntime_Low ? "RuntimeLow" : "RuntimeHigh" );

		std::ostringstream headerContent;
		headerContent << "#pragma once" << std::endl;
		headerContent << std::endl;
		headerContent << "#include \"conct_device.h\"" << std::endl;
		headerContent << "#include \"" << runitmeHeaderFilename << "\"" << std::endl;
		headerContent << std::endl;

		for( const DevicePort& port : device.getPorts() )
		{
			headerContent << "#include \"" << port.pPort->getLibrary() << ".h\"" << std::endl;
		}

		headerContent << std::endl;

		for( const Interface* pInterface : device.getInterfaces() )
		{
			headerContent << "#include \"" << getTypeFilename( pInterface, "proxy", "h" ) << "\"" << std::endl;
		}

		headerContent << std::endl;

		for( const Interface* pInterface : device.getInterfaces() )
		{
			headerContent << "#include \"../" << getTypeFilename( pInterface, "impl", "h" ) << "\"" << std::endl;
		}

		headerContent << std::endl;
		headerContent << "namespace conct" << std::endl;
		headerContent << "{" << std::endl;
		headerContent << "\tclass DeviceInterface" << " : public Device" << std::endl;
		headerContent << "\t{" << std::endl;
		headerContent << "\tpublic:" << std::endl;
		headerContent << std::endl;
		headerContent << "\t\tvoid setupDevice();" << std::endl;
		headerContent << "\t\tvoid loopDevice();" << std::endl;
		headerContent << std::endl;
		headerContent << "\t\tvirtual void getInstances( ArrayView< LocalInstance >& instances ) CONCT_OVERRIDE_FINAL;" << std::endl;
		headerContent << std::endl;
		headerContent << "\tprotected:" << std::endl;
		headerContent << std::endl;
		headerContent << "\t\tvirtual void setup() = 0;" << std::endl;
		headerContent << "\t\tvirtual void loop() = 0;" << std::endl;
		headerContent << std::endl;
		headerContent << "\tprivate:" << std::endl;
		headerContent << std::endl;

		headerContent << "\t\t" << runitmeClassName << " m_runtime;" << std::endl;
		headerContent << std::endl;

		for( const DevicePort& port : device.getPorts() )
		{
			headerContent << "\t\t" << port.pPort->getClass() << " m_port" << port.index << ";" << std::endl;
		}

		headerContent << std::endl;

		for( const Interface* pInterface : device.getProxies() )
		{
			headerContent << "\t\t" << getTypeCppName( pInterface ) << "Proxy m_proxy" << pInterface->getName() << ";" << std::endl;
		}

		headerContent << std::endl;

		for( const DeviceInstance& instance : device.getInstances() )
		{
			headerContent << "\t\t" << getTypeCppName( instance.pInterface ) << "Impl m_instance" << instance.pInterface->getName() << ";" << std::endl;
		}

		headerContent << "\t};" << std::endl;
		headerContent << "}" << std::endl;

		std::ostringstream sourceContent;
		sourceContent << "#include \"device_interface.h\"" << std::endl;
		sourceContent << std::endl;
		sourceContent << "namespace conct" << std::endl;
		sourceContent << "{" << std::endl;

		sourceContent << "\tvoid DeviceInterface::setupDevice()" << std::endl;
		sourceContent << "\t{" << std::endl;

		for( const DevicePort& port : device.getPorts() )
		{
			sourceContent << "\t\tm_port" << port.index << ".setup();" << std::endl;
		}

		sourceContent << std::endl;
		sourceContent << "\t\tm_runtime.setup( this );" << std::endl;
		sourceContent << std::endl;
		sourceContent << "\t\tsetup();" << std::endl;
		sourceContent << "\t}" << std::endl;
		sourceContent << std::endl;

		sourceContent << "\tvoid DeviceInterface::loopDevice()" << std::endl;
		sourceContent << "\t{" << std::endl;

		for( const DevicePort& port : device.getPorts() )
		{
			sourceContent << "\t\tm_port" << port.index << ".loop();" << std::endl;
		}

		sourceContent << std::endl;

		for( const DevicePort& port : device.getPorts() )
		{
			sourceContent << "\t\tm_runtime.processPort( &m_port" << port.index << " );" << std::endl;
		}

		sourceContent << std::endl;
		sourceContent << "\t\tloop();" << std::endl;
		sourceContent << "\t}" << std::endl;
		sourceContent << std::endl;

		sourceContent << "\tvoid DeviceInterface::getInstances( ArrayView< LocalInstance >& instances )" << std::endl;
		sourceContent << "\t{" << std::endl;

		if( device.getInstances().empty() )
		{
			sourceContent << "\t\tinstances.set( nullptr, 0u );" << std::endl;
		}
		else
		{
			sourceContent << "\t\tstatic const LocalInstance s_instances[] =" << std::endl;
			sourceContent << "\t\t{" << std::endl;

			for( const DeviceInstance& instance : device.getInstances() )
			{
				sourceContent << "\t\t\t{ " << instance.id << ", &m_instance" << instance.pInterface->getName() << ", &m_proxy" << instance.pInterface->getName() << " }," << std::endl;
			}

			sourceContent << "\t\t};" << std::endl;
			sourceContent << std::endl;
			sourceContent << "\t\tinstances.set( s_instances, CONCT_COUNT( s_instances ) );" << std::endl;
		}

		sourceContent << "\t}" << std::endl;
		sourceContent << "}" << std::endl;

		const std::filesystem::path headerFilepath = parameters.genOutputPath / "device_interface.h";
		const std::filesystem::path sourceFilepath = parameters.genOutputPath / "device_interface.cpp";
		return writeStringToFile( headerFilepath, headerContent.str(), false ) &&
			writeStringToFile( sourceFilepath, sourceContent.str(), false );
	}

	bool Generator::exportDeviceImpl( const Device& device, const GeneratorParameters& parameters )
	{
		const std::string deviceClassName = "Device" + device.getName();

		std::ostringstream headerContent;
		headerContent << "#pragma once" << std::endl;
		headerContent << std::endl;
		headerContent << "#include \"gen/device_interface.h\"" << std::endl;
		headerContent << std::endl;
		headerContent << "namespace conct" << std::endl;
		headerContent << "{" << std::endl;
		headerContent << "\tclass " << deviceClassName << " : public DeviceInterface" << std::endl;
		headerContent << "\t{" << std::endl;
		headerContent << "\tprotected:" << std::endl;
		headerContent << std::endl;
		headerContent << "\t\tvirtual void setup() CONCT_OVERRIDE_FINAL;" << std::endl;
		headerContent << "\t\tvirtual void loop() CONCT_OVERRIDE_FINAL;" << std::endl;
		headerContent << "\t};" << std::endl;
		headerContent << "}" << std::endl;

		std::ostringstream sourceContent;
		sourceContent << "#include \"device_impl.h\"" << std::endl;
		sourceContent << std::endl;
		sourceContent << "namespace conct" << std::endl;
		sourceContent << "{" << std::endl;
		sourceContent << "\tvoid " << deviceClassName << "::setup()" << std::endl;
		sourceContent << "\t{" << std::endl;
		sourceContent << "\t\t// TODO: implement" << std::endl;
		sourceContent << "\t}" << std::endl;
		sourceContent << std::endl;
		sourceContent << "\tvoid " << deviceClassName << "::loop()" << std::endl;
		sourceContent << "\t{" << std::endl;
		sourceContent << "\t\t// TODO: implement" << std::endl;
		sourceContent << "\t}" << std::endl;
		sourceContent << "}" << std::endl;

		const std::filesystem::path headerFilepath = parameters.outputPath / "device_impl.h";
		const std::filesystem::path sourceFilepath = parameters.outputPath / "device_impl.cpp";
		return writeStringToFile( headerFilepath, headerContent.str(), true ) &&
			writeStringToFile( sourceFilepath, sourceContent.str(), true );
	}

	bool Generator::exportIno( const Device& device, const GeneratorParameters& parameters )
	{
		if( device.getHardware().getRuntime() == HardwareRuntime_High )
		{
			return true;
		}

		const std::string deviceClassName = "Device" + device.getName();

		std::ostringstream headerContent;
		headerContent << "#pragma once" << std::endl;
		headerContent << std::endl;

		for( const std::filesystem::path& file : m_exportedFiles )
		{
			if( file.parent_path() != parameters.genOutputPath ||
				file.extension() != ".cpp" )
			{
				continue;
			}

			headerContent << "#include \"" << file.filename() << "\"" << std::endl;
		}

		headerContent << std::endl;

		std::ostringstream sourceContent;
		sourceContent << "#include \"device_impl.h\"" << std::endl;
		sourceContent << "#include \"gen/files.h\"" << std::endl;
		sourceContent << std::endl;
		sourceContent << "conct::" << deviceClassName << " device;" << std::endl;
		sourceContent << std::endl;
		sourceContent << "void setup()" << std::endl;
		sourceContent << "{" << std::endl;
		sourceContent << "\tdevice.setupDevice();" << std::endl;
		sourceContent << "}" << std::endl;
		sourceContent << std::endl;
		sourceContent << "void loop()" << std::endl;
		sourceContent << "{" << std::endl;
		sourceContent << "\tdevice.loopDevice();" << std::endl;
		sourceContent << "}" << std::endl;
		sourceContent << std::endl;

		const std::string sourceFilename = getDeviceFilename( device );
		if( parameters.outputPath.filename() != sourceFilename )
		{
			std::cout << "Error: device name(" << sourceFilename << ") is not equals to the device folder name(" << parameters.outputPath.filename() << ")." << std::endl;
			return false;
		}

		const std::filesystem::path sourceFilepath = parameters.outputPath / (sourceFilename + ".ino");
		const std::filesystem::path headerFilepath = parameters.genOutputPath / "files.h";
		return writeStringToFile( headerFilepath, headerContent.str(), false ) &&
			writeStringToFile( sourceFilepath, sourceContent.str(), true );
	}
}
