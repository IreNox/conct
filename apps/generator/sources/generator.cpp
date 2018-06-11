#include "generator.h"

#include "device.h"
#include "hardware.h"
#include "port.h"

#include "conct_array_type.h"
#include "conct_ascii.h"
#include "conct_filesystem.h"
#include "conct_interface_type.h"
#include "conct_path.h"
#include "conct_struct_type.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace conct
{
	struct GeneratorParameters
	{
		Path	deviceFileName;
		Path	outputPath;
		Path	genOutputPath;
	};

	Generator::Generator()
	{
	}

	bool Generator::run( const Vector< DynamicString >& arguments )
	{
		const Path exePath = getExecutableName();
		const Path basePath = exePath.getParent().getParent().getParent().getParent().getParent();
		const Path configPath = basePath.push( "config"_s );

		const Path hardwarePath = configPath.push( "hardware"_s );
		if( !m_hardware.load( hardwarePath ) )
		{
			return false;
		}

		const Path portsPath = configPath.push( "ports"_s );
		if( !m_ports.load( portsPath ) )
		{
			return false;
		}

		const Path typesPath = configPath.push( "types"_s );
		if( !m_types.load( typesPath ) )
		{
			return false;
		}

		GeneratorParameters parameters;
		if( !parseArguments( parameters, arguments ) )
		{
			return false;
		}

		if( !filesystem::exists( parameters.outputPath ) &&
			!filesystem::createDirectory( parameters.outputPath ) )
		{
			std::cout << "Error: Failed to create output directory: " << parameters.outputPath.getGenericPath() << std::endl;
			return false;
		}

		if( !filesystem::exists( parameters.genOutputPath ) &&
			!filesystem::createDirectory( parameters.genOutputPath ) )
		{
			std::cout << "Error: Failed to create gen output directory: " << parameters.genOutputPath.getGenericPath() << std::endl;
			return false;
		}

		Device device;
		if( !device.load( parameters.deviceFileName, m_hardware, m_ports, m_types ) )
		{
			std::cout << "Error: Failed to load device from: " << parameters.deviceFileName.getGenericPath() << std::endl;
			return false;
		}

		if( !exportTypes( device, parameters ) ||
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

	DynamicString Generator::getTypeCppName( const Type* pType ) const
	{
		if( pType == nullptr )
		{
			return "void"_s;
		}

		//DynamicString typeNamespace = pType->getNamespace();
		//replaceString( typeNamespace, ".", "::" );

		DynamicString typeName;
		//if( typeNamespace.isEmpty() )
		{
			typeName = pType->getCppName();
		}
		//else
		//{
		//	typeName = typeNamespace + "::" + pType->getCppName();
		//}

		return typeName;
	}

	DynamicString Generator::getTypeCppReturnName( const Type* pType ) const
	{
		if( pType->getDescription() == TypeDescription_Array )
		{
			const ArrayType* pArrayType = static_cast< const ArrayType* >( pType );
			return "ArrayView< "_s + getTypeCppReturnName( pArrayType->getBaseType() ) + " >";
		}
		//else if( pType->getDescription() == TypeDescription_Interface )
		//{
		//	//return
		//}

		return getTypeCppName( pType );
	}

	DynamicString Generator::getTypeCppArgumentName( const Type* pType ) const
	{
		return getTypeCppName( pType );
	}

	DynamicString Generator::getTypeCppValueSetName( const Type* pType ) const
	{
		if( pType->getDescription() == TypeDescription_Array )
		{
			return "Array"_s;
		}
		else if( pType->getDescription() == TypeDescription_Struct )
		{
			return "Struct"_s;
		}

		return pType->getName();
	}

	DynamicString Generator::getDeviceFilename( const Device& device ) const
	{
		DynamicString filename;

		for( char c : device.getName() )
		{
			if( ascii::isCapital( c ) )
			{
				if( !filename.isEmpty() )
				{
					filename.pushBack( '_' );
				}

				filename.pushBack( ascii::toLower( c ) );
			}
			else
			{
				filename.pushBack( c );
			}
		}

		return filename;
	}

	bool Generator::writeStringToFile( const Path& filename, const DynamicString& content, bool ifNotExists )
	{
		m_exportedFiles.insert( filename );

		{
			std::ifstream readStream( filename.getNativePath().toConstCharPointer() );
			if( readStream.is_open() )
			{
				if( ifNotExists )
				{
					return true;
				}

				DynamicString currentContent( ( std::istreambuf_iterator<char>( readStream ) ), std::istreambuf_iterator<char>() );
				if( currentContent == content )
				{
					return true;
				}
			}

			readStream.close();
		}

		std::ofstream writeStream( filename.getNativePath().toConstCharPointer(), std::ofstream::out | std::ofstream::trunc );
		if( !writeStream.is_open() )
		{
			std::cout << "Error: Failed to open '" << filename.getGenericPath() << "'" << std::endl;
			return false;
		}

		writeStream << content;
		writeStream.close();

		return true;
	}

	bool Generator::parseArguments( GeneratorParameters& parameters, const Vector< DynamicString >& arguments )
	{
		if( arguments.getCount() < 2u )
		{
			std::cout << "Usage: " << arguments[ 0u ] << " INPUT_FILE" << std::endl;
			return false;
		}

		for( const DynamicString& argument : arguments )
		{
			if( argument.isEmpty() || argument == arguments[ 0u ] )
			{
				continue;
			}

			if( argument[ 0u ] == '-' )
			{
				const size_t assignIndex = argument.indexOf( '=' );
				if( assignIndex == InvalidStringIndex )
				{
					std::cout << "Warning: Invalid parameter '" << argument << "'" << std::endl;
					continue;
				}

				const DynamicString argumentName = argument.subString( 0u, assignIndex );
				const DynamicString argumentValue = argument.subString( assignIndex + 1u );
				if( argumentName == "--output" )
				{
					parameters.outputPath = Path( argumentValue );
				}
				else
				{
					std::cout << "Warning: Unknown parameter '" << argumentName << "'" << std::endl;
				}
			}
			else
			{
				if( !parameters.deviceFileName.isEmpty() )
				{
					std::cout << "Warning: Invalid second input file '" << argument << "'" << std::endl;
					continue;
				}

				parameters.deviceFileName = Path( argument );
			}
		}

		if( parameters.deviceFileName.isEmpty() )
		{
			std::cout << "Error: No input file given" << std::endl;
			return false;
		}

		if( parameters.outputPath.isEmpty() )
		{
			parameters.outputPath = parameters.deviceFileName.getParent();
		}

		if( parameters.genOutputPath.isEmpty() )
		{
			parameters.genOutputPath = parameters.outputPath.push( "gen"_s );
		}

		return true;
	}

	void Generator::collectTypesToExport( TypeSet& types, const Device& device )
	{
		for( const Type* pType : device.getProxies() )
		{
			collectTypesToExport( types, pType );
		}

		for( const Type* pType : device.getInterfaces() )
		{
			collectTypesToExport( types, pType );
		}

		for( const Type* pType : device.getStructs() )
		{
			collectTypesToExport( types, pType );
		}

		for( const DeviceInstance& instance : device.getInstances() )
		{
			collectTypesToExport( types, instance.pInterface );
		}
	}

	void Generator::collectTypesToExport( TypeSet& types, const Type* pType )
	{
		if( pType->getDescription() == TypeDescription_Value )
		{
			return;
		}

		if( !types.insert( pType ) )
		{
			return;
		}

		for( const Type* pDependingType : pType->getDependingTypes() )
		{
			collectTypesToExport( types, pDependingType );
		}
	}

	bool Generator::exportTypes( const Device& device, const GeneratorParameters& parameters )
	{
		Type::TypeSet types;
		collectTypesToExport( types, device );

		for( const Type* pType : types )
		{
			if( pType->getDescription() == TypeDescription_Struct )
			{
				const StructType* pStruct = static_cast< const StructType* >( pType );
				if( !exportStruct( device, parameters, pStruct ) )
				{
					return false;
				}
			}
			else if( pType->getDescription() == TypeDescription_Interface )
			{
				const InterfaceType* pInterface = static_cast< const InterfaceType* >( pType );
				if( !exportInterface( device, parameters, pInterface ) )
				{
					return false;
				}
			}
		}

		return true;
	}

	bool Generator::exportStruct( const Device& device, const GeneratorParameters& parameters, const StructType* pStruct )
	{
		std::ostringstream headerContent;
		headerContent << "#pragma once" << std::endl;
		headerContent << std::endl;

		writeDependingTypeIncludes( headerContent, pStruct );

		headerContent << std::endl;
		headerContent << "namespace conct" << std::endl;
		headerContent << "{" << std::endl;
		headerContent << "\tstruct " << pStruct->getName() << std::endl;
		headerContent << "\t{" << std::endl;
		headerContent << "\t\tstatic const TypeCrc s_typeCrc = " << pStruct->getCrc() << ";" << std::endl;
		headerContent << std::endl;

		for( const StructField& field : pStruct->getFields() )
		{
			headerContent << "\t\t" << getTypeCppReturnName( field.pType ) << " " << field.name << ";" << std::endl;
		}

		headerContent << "\t};" << std::endl;
		headerContent << "}" << std::endl;

		const Path headerFilepath = parameters.genOutputPath.push( pStruct->getHeaderFilename() );
		if( !writeStringToFile( headerFilepath, DynamicString( headerContent.str().c_str() ), false ) )
		{
			return false;
		}

		return true;
	}

	bool Generator::exportInterface( const Device& device, const GeneratorParameters& parameters, const InterfaceType* pInterface )
	{
		if( pInterface->getInternal() )
		{
			return true;
		}

		std::ostringstream headerContent;
		headerContent << "#pragma once" << std::endl;
		headerContent << std::endl;

		writeDependingTypeIncludes( headerContent, pInterface );

		headerContent << std::endl;
		headerContent << "namespace conct" << std::endl;
		headerContent << "{" << std::endl;
		headerContent << "\tclass " << pInterface->getName() << std::endl;
		headerContent << "\t{" << std::endl;
		headerContent << "\tpublic:" << std::endl;
		headerContent << std::endl;
		headerContent << "\t\tstatic const TypeCrc s_typeCrc = " << pInterface->getCrc() << ";" << std::endl;
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

		if( !pInterface->getProperties().isEmpty() && !pInterface->getFunctions().isEmpty() )
		{
			headerContent << std::endl;
		}

		for( const InterfaceFunction& function : pInterface->getFunctions() )
		{
			DynamicString parameters;
			for( const InterfaceFunctionParameter& parameter : function.parameters )
			{
				if( !parameters.isEmpty() )
				{
					parameters += ", ";
				}

				parameters += getTypeCppArgumentName( parameter.pType ) + " " + parameter.name;
			}

			headerContent << "\t\tvirtual " << getTypeCppReturnName( function.pReturnType ) << " " << function.name << "( " << parameters << " ) = 0;" << std::endl;
		}

		headerContent << "\t};" << std::endl;
		headerContent << "}" << std::endl;

		const std::filesystem::path headerFilepath = parameters.genOutputPath / pInterface->getHeaderFilename();
		if( !writeStringToFile( headerFilepath, headerContent.str(), false ) )
		{
			return false;
		}

		return true;
	}

	bool Generator::exportInterfaceProxies( const Device& device, const GeneratorParameters& parameters )
	{
		for( const InterfaceType* pInterface : device.getProxies() )
		{
			const DynamicString headerFilename = pInterface->getFilename( "proxy", "h" );
			const DynamicString sourceFilename = pInterface->getFilename( "proxy", "cpp" );

			const DynamicString proxyName = pInterface->getName() + "Proxy";

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
			headerContent << "\t\tvirtual bool getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, const char* pName ) const CONCT_OVERRIDE_FINAL;" << std::endl;
			headerContent << "\t\tvirtual bool setProperty( void* pInstance, const char* pName, const Value& value ) const CONCT_OVERRIDE_FINAL;" << std::endl;
			headerContent << std::endl;
			headerContent << "\t\tvirtual bool callFunction( ValueBuilder& targetValueBuilder, void* pInstance, const char* pName, const ArrayView< Value >& parameters ) const CONCT_OVERRIDE_FINAL;" << std::endl;
			headerContent << "\t};" << std::endl;
			headerContent << "}" << std::endl;

			std::ostringstream sourceContent;
			sourceContent << "#include \"" << headerFilename << "\"" << std::endl;
			sourceContent << std::endl;
			sourceContent << "#include \"conct_string.h\"" << std::endl;
			sourceContent << std::endl;

			sourceContent << "#include \"" << pInterface->getHeaderFilename() << "\"" << std::endl;
			writeDependingTypeIncludes( sourceContent, pInterface );

			sourceContent << std::endl;
			sourceContent << "namespace conct" << std::endl;
			sourceContent << "{" << std::endl;

			sourceContent << "\t" << proxyName << "::" << proxyName << "()" << std::endl;

			if( pInterface->getInternal() )
			{
				sourceContent << "\t\t: Proxy( " << pInterface->getCrc() << " )" << std::endl;
			}
			else
			{
				sourceContent << "\t\t: Proxy( " << pInterface->getCppName() << "::s_typeCrc )" << std::endl;
			}

			sourceContent << "\t{" << std::endl;
			sourceContent << "\t}" << std::endl;
			sourceContent << std::endl;

			sourceContent << "\tbool " << proxyName << "::getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, const char* pName ) const" << std::endl;
			sourceContent << "\t{" << std::endl;
			sourceContent << "\t\tconst " << pInterface->getName() << "* pTypedInstance = static_cast< const " << pInterface->getName() << "* >( pInstance );" << std::endl;
			sourceContent << std::endl;

			for( const InterfaceProperty& property : pInterface->getProperties() )
			{
				if( !property.hasGetter )
				{
					continue;
				}

				sourceContent << "\t\tif( isStringEquals( pName, \"" << property.name << "\" ) )" << std::endl;
				sourceContent << "\t\t{" << std::endl;
				sourceContent << "\t\t\ttargetValueBuilder.set" << getTypeCppValueSetName( property.pType ) << "( pTypedInstance->get" << property.name << "() );" << std::endl;
				sourceContent << "\t\t\treturn true;" << std::endl;
				sourceContent << "\t\t}" << std::endl;
			}

			sourceContent << std::endl;
			sourceContent << "\t\treturn false;" << std::endl;
			sourceContent << "\t}" << std::endl;
			sourceContent << std::endl;

			sourceContent << "\tbool " << proxyName << "::setProperty( void* pInstance, const char* pName, const Value& value ) const" << std::endl;
			sourceContent << "\t{" << std::endl;
			sourceContent << "\t\t" << pInterface->getName() << "* pTypedInstance = static_cast< " << pInterface->getName() << "* >( pInstance );" << std::endl;
			sourceContent << std::endl;

			for( const InterfaceProperty& property : pInterface->getProperties() )
			{
				if( !property.hasSetter )
				{
					continue;
				}

				sourceContent << "\t\tif( isStringEquals( pName, \"" << property.name << "\" ) )" << std::endl;
				sourceContent << "\t\t{" << std::endl;
				sourceContent << "\t\t\tpTypedInstance->set" << property.name << "( value.get" <<  property.pType->getName() << "() );" << std::endl;
				sourceContent << "\t\t\treturn true;" << std::endl;
				sourceContent << "\t\t}" << std::endl;
			}

			sourceContent << std::endl;
			sourceContent << "\t\treturn false;" << std::endl;
			sourceContent << "\t}" << std::endl;
			sourceContent << std::endl;

			sourceContent << "\tbool " << pInterface->getName() << "Proxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, const char* pName, const ArrayView< Value >& parameters ) const" << std::endl;
			sourceContent << "\t{" << std::endl;
			sourceContent << "\t\t" << pInterface->getName() << "* pTypedInstance = static_cast< " << pInterface->getName() << "* >( pInstance );" << std::endl;
			sourceContent << std::endl;

			for( const InterfaceFunction& function : pInterface->getFunctions() )
			{
				sourceContent << "\t\tif( isStringEquals( pName, \"" << function.name << "\" ) )" << std::endl;
				sourceContent << "\t\t{" << std::endl;

				std::ostringstream parameters;

				size_t i = 0u;
				for( const InterfaceFunctionParameter& parameter : function.parameters )
				{
					if( i == 0u )
					{
						parameters << " ";
					}
					else
					{
						parameters << ", ";
					}

					parameters << "parameters[ " << i << "u ].get" << getTypeCppValueSetName( parameter.pType ) << "()";
					i++;
				}

				if( !function.parameters.isEmpty() )
				{
					parameters << " ";
				}

				sourceContent << "\t\t\ttargetValueBuilder.set" << getTypeCppValueSetName( function.pReturnType ) << "( pTypedInstance->" << function.name << "(" << parameters.str() << ") );" << std::endl;
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
			const InterfaceType* pInterface = instance.pInterface;
			if( pInterface->getInternal() )
			{
				continue;
			}

			const DynamicString headerFilename = pInterface->getFilename( "impl", "h" );
			const DynamicString sourceFilename = pInterface->getFilename( "impl", "cpp" );

			const DynamicString className = pInterface->getName() + "Impl";

			std::ostringstream headerContent;
			headerContent << "#pragma once" << std::endl;
			headerContent << std::endl;
			headerContent << "#include \"gen/" << pInterface->getHeaderFilename() << "\"" << std::endl;
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

			if( !pInterface->getProperties().isEmpty() && !pInterface->getFunctions().isEmpty() )
			{
				headerContent << std::endl;
			}

			for( const InterfaceFunction& function : pInterface->getFunctions() )
			{
				DynamicString parameters;
				for( const InterfaceFunctionParameter& parameter : function.parameters )
				{
					if( !parameters.isEmpty() )
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

			if( !pInterface->getProperties().isEmpty() && !pInterface->getFunctions().isEmpty() )
			{
				sourceContent << std::endl;
			}

			for( const InterfaceFunction& function : pInterface->getFunctions() )
			{
				DynamicString parameters;
				for( const InterfaceFunctionParameter& parameter : function.parameters )
				{
					if( !parameters.isEmpty() )
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
		const InterfaceType* pDeviceInterface = m_types.findInterface( "Core.Device", "" );

		const HardwareRuntime runtime = device.getHardware().getRuntime();
		const DynamicString runitmeHeaderFilename = (runtime == HardwareRuntime_Low ? "conct_runtime_low.h" : "conct_runtime_high.h");
		const DynamicString runitmeClassName = ( runtime == HardwareRuntime_Low ? "RuntimeLow" : "RuntimeHigh" );

		std::ostringstream headerContent;
		headerContent << "#pragma once" << std::endl;
		headerContent << std::endl;
		headerContent << "#include \"" << pDeviceInterface->getHeaderFilename() << "\"" << std::endl;
		headerContent << "#include \"" << runitmeHeaderFilename << "\"" << std::endl;
		headerContent << std::endl;

		for( const DevicePort& port : device.getPorts() )
		{
			headerContent << "#include \"" << port.pPort->getLibrary() << ".h\"" << std::endl;
		}

		headerContent << std::endl;

		for( const InterfaceType* pInterface : device.getInterfaces() )
		{
			headerContent << "#include \"" << pInterface->getFilename( "proxy", "h" ) << "\"" << std::endl;
		}

		headerContent << std::endl;

		for( const InterfaceType* pInterface : device.getInterfaces() )
		{
			if( pInterface == pDeviceInterface )
			{
				continue;
			}
			else if( pInterface->getInternal() )
			{
				headerContent << "#include \"" << pInterface->getHeaderFilename() << "\"" << std::endl;
			}
			else
			{
				headerContent << "#include \"../" << pInterface->getFilename( "impl", "h" ) << "\"" << std::endl;
			}
		}

		headerContent << std::endl;
		headerContent << "namespace conct" << std::endl;
		headerContent << "{" << std::endl;
		headerContent << "\tclass DeviceInterface" << " : public " << pDeviceInterface->getCppName() << std::endl;
		headerContent << "\t{" << std::endl;
		headerContent << "\tpublic:" << std::endl;
		headerContent << std::endl;
		headerContent << "\t\tvoid setupDevice();" << std::endl;
		headerContent << "\t\tvoid loopDevice();" << std::endl;
		headerContent << std::endl;
		headerContent << "\t\tvirtual const char* getName() const CONCT_OVERRIDE_FINAL;" << std::endl;
		headerContent << std::endl;
		headerContent << "\tprotected:" << std::endl;
		headerContent << std::endl;
		headerContent << "\t\t" << runitmeClassName << " m_runtime;" << std::endl;
		headerContent << std::endl;
		headerContent << "\t\tvirtual void setup() = 0;" << std::endl;
		headerContent << "\t\tvirtual void loop() = 0;" << std::endl;
		headerContent << std::endl;
		headerContent << "\t\tvirtual void getEmptyInstances( Array< Instance >& instances ) CONCT_OVERRIDE_FINAL;" << std::endl;
		headerContent << "\t\tvirtual void getPublicInstances( ArrayView< Instance >& instances ) const CONCT_OVERRIDE_FINAL;" << std::endl;
		headerContent << "\t\tvirtual void getLocalInstances( ArrayView< LocalInstance >& instances ) CONCT_OVERRIDE_FINAL;" << std::endl;
		headerContent << std::endl;
		headerContent << "\tprivate:" << std::endl;
		headerContent << std::endl;

		for( const DevicePort& port : device.getPorts() )
		{
			headerContent << "\t\t" << port.pPort->getClass() << " m_port" << port.index << ";" << std::endl;
		}

		headerContent << std::endl;

		for( const InterfaceType* pInterface : device.getProxies() )
		{
			headerContent << "\t\t" << getTypeCppName( pInterface ) << "Proxy m_proxy" << pInterface->getName() << ";" << std::endl;
		}

		headerContent << std::endl;

		for( const DeviceInstance& instance : device.getInstances() )
		{
			if( instance.pInterface == pDeviceInterface )
			{
				continue;
			}
			else if( instance.pInterface->getInternal() )
			{
				headerContent << "\t\t" << getTypeCppName( instance.pInterface ) << " m_instance" << instance.pInterface->getName() << ";" << std::endl;
			}
			else
			{
				headerContent << "\t\t" << getTypeCppName( instance.pInterface ) << "Impl m_instance" << instance.pInterface->getName() << ";" << std::endl;
			}
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

		if( runtime == HardwareRuntime_High )
		{
			for( const DevicePort& port : device.getPorts() )
			{
				sourceContent << "\t\tm_runtime.registerPort( &m_port" << port.index << " );" << std::endl;
			}
		}

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

		sourceContent << "\tconst char* DeviceInterface::getName() const" << std::endl;
		sourceContent << "\t{" << std::endl;
		sourceContent << "\t\t return \"" << device.getName() << "\";" << std::endl;
		sourceContent << "\t}" << std::endl;
		sourceContent << std::endl;

		sourceContent << "\tvoid DeviceInterface::getEmptyInstances( Array< Instance >& instances )" << std::endl;
		sourceContent << "\t{" << std::endl;

		if( device.getInstances().isEmpty() )
		{
			sourceContent << "\t\tinstances.set( nullptr, 0u );" << std::endl;
		}
		else
		{
			sourceContent << "\t\tstatic Instance s_instances[ "<< device.getInstances().size() << "u ];" << std::endl;
			sourceContent << "\t\tinstances.set( s_instances, CONCT_COUNT( s_instances ) );" << std::endl;
		}

		sourceContent << "\t}" << std::endl;
		sourceContent << std::endl;

		sourceContent << "\tvoid DeviceInterface::getPublicInstances( ArrayView< Instance >& instances ) const" << std::endl;
		sourceContent << "\t{" << std::endl;

		if( device.getInstances().isEmpty() )
		{
			sourceContent << "\t\tinstances.set( nullptr, 0u );" << std::endl;
		}
		else
		{
			sourceContent << "\t\tstatic const Instance s_instances[] =" << std::endl;
			sourceContent << "\t\t{" << std::endl;

			for( const DeviceInstance& instance : device.getInstances() )
			{
				sourceContent << "\t\t\t{ " << instance.id << ", " << instance.pInterface->getCrc() << " }," << std::endl;
			}

			sourceContent << "\t\t};" << std::endl;
			sourceContent << std::endl;
			sourceContent << "\t\tinstances.set( s_instances, CONCT_COUNT( s_instances ) );" << std::endl;
		}

		sourceContent << "\t}" << std::endl;
		sourceContent << std::endl;

		sourceContent << "\tvoid DeviceInterface::getLocalInstances( ArrayView< LocalInstance >& instances )" << std::endl;
		sourceContent << "\t{" << std::endl;

		if( device.getInstances().isEmpty() )
		{
			sourceContent << "\t\tinstances.set( nullptr, 0u );" << std::endl;
		}
		else
		{
			sourceContent << "\t\tstatic const LocalInstance s_instances[] =" << std::endl;
			sourceContent << "\t\t{" << std::endl;

			for( const DeviceInstance& instance : device.getInstances() )
			{
				if( instance.pInterface == pDeviceInterface )
				{
					sourceContent << "\t\t\t{ " << instance.id << ", this, &m_proxy" << instance.pInterface->getName() << " }," << std::endl;
				}
				else
				{
					sourceContent << "\t\t\t{ " << instance.id << ", &m_instance" << instance.pInterface->getName() << ", &m_proxy" << instance.pInterface->getName() << " }," << std::endl;
				}
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
		const DynamicString deviceClassName = "Device" + device.getName();

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

		const DynamicString deviceClassName = "Device" + device.getName();

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

		const DynamicString sourceFilename = getDeviceFilename( device );
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

	void Generator::writeDependingTypeIncludes( std::ostringstream& target, const Type* pType )
	{
		std::set<DynamicString > headerFiles;

		for( const Type* pDependingType : pType->getDependingTypes() )
		{
			headerFiles.insert( pDependingType->getHeaderFilename() );
		}

		for( const DynamicString& headerFile : headerFiles )
		{
			target << "#include \"" << headerFile << "\"" << std::endl;
		}
	}
}
