#include "generator.h"

#include "device.h"
#include "hardware.h"
#include "port.h"

#include "conct_array_type.h"
#include "conct_ascii.h"
#include "conct_filesystem.h"
#include "conct_interface_type.h"
#include "conct_path.h"
#include "conct_string_tools.h"
#include "conct_struct_type.h"
#include "conct_trace.h"

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
		const Path exePath = Path::getExecutablePath();
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

		Result< void > createDirResult = filesystem::createDirectory( parameters.outputPath );
		if( createDirResult.isFailure() && createDirResult.result != ResultId_AlreadyExists )
		{
			trace::write( "Error: Failed to create output directory: "_s + parameters.outputPath.getGenericPath() + ". Reason: " + createDirResult.getResultString() + "\n" );
			return false;
		}

		createDirResult = filesystem::createDirectory( parameters.genOutputPath );
		if( createDirResult.isFailure() && createDirResult.result != ResultId_AlreadyExists )
		{
			trace::write( "Error: Failed to create gen output directory: "_s + parameters.genOutputPath.getGenericPath() + ". Reason: " + createDirResult.getResultString() + "\n" );
			return false;
		}

		Device device;
		if( !device.load( parameters.deviceFileName, m_hardware, m_ports, m_types ) )
		{
			trace::write( "Error: Failed to load device from: "_s + parameters.deviceFileName.getGenericPath() + "\n" );
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
		DynamicString filename = device.getName();

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
			const Result< DynamicString > readResult = filesystem::readTextFile( filename );
			if( readResult.isSuccess() )
			{
				if( ifNotExists )
				{
					return true;
				}

				if( readResult.value == content )
				{
					return true;
				}
			}
		}

		const Result< void > writeResult = filesystem::writeTextFile( filename, content );
		if( writeResult.isFailure() )
		{
			trace::write( "Error: Failed to open '"_s + filename.getGenericPath() + "'. Reason: " + writeResult.getResultString() + "\n" );
			return false;
		}

		return true;
	}

	bool Generator::parseArguments( GeneratorParameters& parameters, const Vector< DynamicString >& arguments )
	{
		if( arguments.getLength() < 2u )
		{
			trace::write( "Usage: "_s + arguments[ 0u ] + " INPUT_FILE\n" );
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
					trace::write( "Warning: Invalid parameter '"_s + argument + "'\n" );
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
					trace::write( "Warning: Unknown parameter '"_s + argumentName + "'\n" );
				}
			}
			else
			{
				if( !parameters.deviceFileName.isEmpty() )
				{
					trace::write( "Warning: Invalid second input file '"_s + argument + "'\n" );
					continue;
				}

				parameters.deviceFileName = Path( argument );
			}
		}

		if( parameters.deviceFileName.isEmpty() )
		{
			trace::write( "Error: No input file given\n" );
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
		DynamicString headerContent;
		headerContent += "#pragma once\n";
		headerContent += "\n";

		writeDependingTypeIncludes( headerContent, pStruct );

		headerContent += "\n";
		headerContent += "namespace conct\n";
		headerContent += "{\n";
		headerContent += "\tstruct "_s + pStruct->getName() + "\n";
		headerContent += "\t{\n";
		headerContent += "\t\tstatic const TypeCrc s_typeCrc = "_s + string_tools::toString( pStruct->getCrc() ) + ";\n";
		headerContent += "\n";

		for( const StructField& field : pStruct->getFields() )
		{
			headerContent += "\t\t"_s + getTypeCppReturnName( field.pType ) + " " + field.name + ";\n";
		}

		headerContent += "\t};\n";
		headerContent += "}\n";

		const Path headerFilepath = parameters.genOutputPath.push( pStruct->getHeaderFilename() );
		if( !writeStringToFile( headerFilepath, headerContent, false ) )
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

		DynamicString headerContent;
		headerContent += "#pragma once\n";
		headerContent += "\n";

		writeDependingTypeIncludes( headerContent, pInterface );

		headerContent += "\n";
		headerContent += "namespace conct\n";
		headerContent += "{\n";
		headerContent += "\tclass "_s + pInterface->getName() + "\n";
		headerContent += "\t{\n";
		headerContent += "\tpublic:\n";
		headerContent += "\n";
		headerContent += "\t\tstatic const TypeCrc s_typeCrc = "_s + string_tools::toString( pInterface->getCrc() ) + ";\n";
		headerContent += "\n";

		for( const InterfaceProperty& property : pInterface->getProperties() )
		{
			if( property.hasGetter )
			{
				headerContent += "\t\tvirtual "_s + getTypeCppReturnName( property.pType ) + " get" + property.name + "() const = 0;\n";
			}

			if( property.hasSetter )
			{
				headerContent += "\t\tvirtual void set"_s + property.name + "( " + getTypeCppArgumentName( property.pType ) + " value ) = 0;\n";
			}
		}

		if( !pInterface->getProperties().isEmpty() && !pInterface->getFunctions().isEmpty() )
		{
			headerContent += "\n";
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

			headerContent += "\t\tvirtual "_s + getTypeCppReturnName( function.pReturnType ) + " " + function.name + "( " + parameters + " ) = 0;\n";
		}

		headerContent += "\t};\n";
		headerContent += "}\n";

		const Path headerFilepath = parameters.genOutputPath.push( pInterface->getHeaderFilename() );
		if( !writeStringToFile( headerFilepath, headerContent, false ) )
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

			DynamicString headerContent;
			headerContent += "#pragma once\n";
			headerContent += "\n";
			headerContent += "#include \"conct_proxy.h\"\n";
			headerContent += "\n";
			headerContent += "namespace conct\n";
			headerContent += "{\n";
			headerContent += "\tclass "_s + proxyName + " : public Proxy\n";
			headerContent += "\t{\n";
			headerContent += "\tpublic:\n";
			headerContent += "\n";
			headerContent += "\t\t"_s + proxyName + "();\n";
			headerContent += "\n";
			headerContent += "\t\tvirtual bool getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, const char* pName ) const CONCT_OVERRIDE_FINAL;\n";
			headerContent += "\t\tvirtual bool setProperty( void* pInstance, const char* pName, const Value& value ) const CONCT_OVERRIDE_FINAL;\n";
			headerContent += "\n";
			headerContent += "\t\tvirtual bool callFunction( ValueBuilder& targetValueBuilder, void* pInstance, const char* pName, const ArrayView< Value >& parameters ) const CONCT_OVERRIDE_FINAL;\n";
			headerContent += "\t};\n";
			headerContent += "}\n";

			DynamicString sourceContent;
			sourceContent += "#include \""_s + headerFilename + "\"\n";
			sourceContent += "\n";
			sourceContent += "#include \"conct_string.h\"\n";
			sourceContent += "\n";

			sourceContent += "#include \""_s + pInterface->getHeaderFilename() + "\"\n";
			writeDependingTypeIncludes( sourceContent, pInterface );

			sourceContent += "\n";
			sourceContent += "namespace conct\n";
			sourceContent += "{\n";

			sourceContent += "\t"_s + proxyName + "::" + proxyName + "()\n";

			if( pInterface->getInternal() )
			{
				sourceContent += "\t\t: Proxy( "_s + string_tools::toString( pInterface->getCrc() ) + " )\n";
			}
			else
			{
				sourceContent += "\t\t: Proxy( "_s + pInterface->getCppName() + "::s_typeCrc )\n";
			}

			sourceContent += "\t{\n";
			sourceContent += "\t}\n";
			sourceContent += "\n";

			sourceContent += "\tbool "_s + proxyName + "::getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, const char* pName ) const\n";
			sourceContent += "\t{\n";
			sourceContent += "\t\tconst "_s + pInterface->getName() + "* pTypedInstance = static_cast< const " + pInterface->getName() + "* >( pInstance );\n";
			sourceContent += "\n";

			for( const InterfaceProperty& property : pInterface->getProperties() )
			{
				if( !property.hasGetter )
				{
					continue;
				}

				sourceContent += "\t\tif( isStringEquals( pName, \""_s + property.name + "\" ) )\n";
				sourceContent += "\t\t{\n";
				sourceContent += "\t\t\ttargetValueBuilder.set"_s + getTypeCppValueSetName( property.pType ) + "( pTypedInstance->get" + property.name + "() );\n";
				sourceContent += "\t\t\treturn true;\n";
				sourceContent += "\t\t}\n";
			}

			sourceContent += "\n";
			sourceContent += "\t\treturn false;\n";
			sourceContent += "\t}\n";
			sourceContent += "\n";

			sourceContent += "\tbool "_s + proxyName + "::setProperty( void* pInstance, const char* pName, const Value& value ) const\n";
			sourceContent += "\t{\n";
			sourceContent += "\t\t"_s + pInterface->getName() + "* pTypedInstance = static_cast< " + pInterface->getName() + "* >( pInstance );\n";
			sourceContent += "\n";

			for( const InterfaceProperty& property : pInterface->getProperties() )
			{
				if( !property.hasSetter )
				{
					continue;
				}

				sourceContent += "\t\tif( isStringEquals( pName, \""_s + property.name + "\" ) )\n";
				sourceContent += "\t\t{\n";
				sourceContent += "\t\t\tpTypedInstance->set"_s + property.name + "( value.get" +  property.pType->getName() + "() );\n";
				sourceContent += "\t\t\treturn true;\n";
				sourceContent += "\t\t}\n";
			}

			sourceContent += "\n";
			sourceContent += "\t\treturn false;\n";
			sourceContent += "\t}\n";
			sourceContent += "\n";

			sourceContent += "\tbool "_s + pInterface->getName() + "Proxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, const char* pName, const ArrayView< Value >& parameters ) const\n";
			sourceContent += "\t{\n";
			sourceContent += "\t\t"_s + pInterface->getName() + "* pTypedInstance = static_cast< " + pInterface->getName() + "* >( pInstance );\n";
			sourceContent += "\n";

			for( const InterfaceFunction& function : pInterface->getFunctions() )
			{
				sourceContent += "\t\tif( isStringEquals( pName, \""_s + function.name + "\" ) )\n";
				sourceContent += "\t\t{\n";

				DynamicString parameters;

				size_t i = 0u;
				for( const InterfaceFunctionParameter& parameter : function.parameters )
				{
					if( i == 0u )
					{
						parameters += " ";
					}
					else
					{
						parameters += ", ";
					}

					parameters += "parameters[ "_s + string_tools::toString( i ) + "u ].get" + getTypeCppValueSetName( parameter.pType ) + "()";
					i++;
				}

				if( !function.parameters.isEmpty() )
				{
					parameters += " ";
				}

				sourceContent += "\t\t\ttargetValueBuilder.set"_s + getTypeCppValueSetName( function.pReturnType ) + "( pTypedInstance->" + function.name + "(" + parameters + ") );\n";
				sourceContent += "\t\t\treturn true;\n";
				sourceContent += "\t\t}\n";
			}

			sourceContent += "\n";
			sourceContent += "\t\treturn false;\n";
			sourceContent += "\t}\n";

			sourceContent += "}\n";

			const Path headerFilepath = parameters.genOutputPath.push( headerFilename );
			const Path sourceFilepath = parameters.genOutputPath.push( sourceFilename );
			if( !writeStringToFile( headerFilepath, headerContent, false ) ||
				!writeStringToFile( sourceFilepath, sourceContent, false ) )
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

			DynamicString headerContent;
			headerContent += "#pragma once\n";
			headerContent += "\n";
			headerContent += "#include \"gen/"_s + pInterface->getHeaderFilename() + "\"\n";
			headerContent += "\n";
			headerContent += "namespace conct\n";
			headerContent += "{\n";
			headerContent += "\tclass "_s + className + " : " + pInterface->getName() + "\n";
			headerContent += "\t{\n";
			headerContent += "\tpublic:\n";
			headerContent += "\n";

			for( const InterfaceProperty& property : pInterface->getProperties() )
			{
				if( property.hasGetter )
				{
					headerContent += "\t\tvirtual "_s + getTypeCppReturnName( property.pType ) + " get" + property.name + "() const CONCT_OVERRIDE_FINAL;\n";
				}

				if( property.hasSetter )
				{
					headerContent += "\t\tvirtual void set"_s + property.name + "( " + getTypeCppArgumentName( property.pType ) + " value ) CONCT_OVERRIDE_FINAL;\n";
				}
			}

			if( !pInterface->getProperties().isEmpty() && !pInterface->getFunctions().isEmpty() )
			{
				headerContent += "\n";
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

				headerContent += "\t\tvirtual "_s + getTypeCppReturnName( function.pReturnType ) + " " + function.name + "( " + parameters + " ) CONCT_OVERRIDE_FINAL;\n";
			}

			headerContent += "\t};\n";
			headerContent += "}\n";

			DynamicString sourceContent;
			sourceContent += "#include \""_s + headerFilename + "\"\n";
			sourceContent += "\n";
			sourceContent += "namespace conct\n";
			sourceContent += "{\n";

			for( const InterfaceProperty& property : pInterface->getProperties() )
			{
				if( property.hasGetter )
				{
					sourceContent += "\t"_s + getTypeCppReturnName( property.pType ) + " " + className + "::get" + property.name + "() const\n";
					sourceContent += "\t{\n";
					sourceContent += "\t\t// TODO: implement\n";
					sourceContent += "\t}\n";
					sourceContent += "\n";
				}

				if( property.hasSetter )
				{
					sourceContent += "\tvoid "_s + className + "::set" + property.name + "( " + getTypeCppArgumentName( property.pType ) + " value )\n";
					sourceContent += "\t{\n";
					sourceContent += "\t\t// TODO: implement\n";
					sourceContent += "\t}\n";
					sourceContent += "\n";
				}
			}

			if( !pInterface->getProperties().isEmpty() && !pInterface->getFunctions().isEmpty() )
			{
				sourceContent += "\n";
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

				sourceContent += "\t"_s + getTypeCppReturnName( function.pReturnType ) + " " + className + "::" + function.name + "( " + parameters + " )\n";
				sourceContent += "\t{\n";
				sourceContent += "\t\t// TODO: implement\n";
				sourceContent += "\t}\n";
				sourceContent += "\n";
			}

			sourceContent += "}\n";

			const Path headerFilepath = parameters.outputPath.push( headerFilename );
			const Path sourceFilepath = parameters.outputPath.push( sourceFilename );
			if( !writeStringToFile( headerFilepath, headerContent, true ) ||
				!writeStringToFile( sourceFilepath, sourceContent, true ) )
			{
				return false;
			}
		}

		return true;
	}

	bool Generator::exportDeviceInterface( const Device& device, const GeneratorParameters& parameters )
	{
		const InterfaceType* pDeviceInterface = m_types.findInterface( "Core.Device"_s, ""_s );

		const HardwareRuntime runtime = device.getHardware().getRuntime();
		const DynamicString runitmeHeaderFilename = DynamicString( runtime == HardwareRuntime_Low ? "conct_runtime_low.h" : "conct_runtime_high.h" );
		const DynamicString runitmeClassName = DynamicString( runtime == HardwareRuntime_Low ? "RuntimeLow" : "RuntimeHigh" );

		DynamicString headerContent;
		headerContent += "#pragma once\n";
		headerContent += "\n";
		headerContent += "#include \""_s + pDeviceInterface->getHeaderFilename() + "\"\n";
		headerContent += "#include \""_s + runitmeHeaderFilename + "\"\n";
		headerContent += "\n";

		for( const DevicePort& port : device.getPorts() )
		{
			headerContent += "#include \""_s + port.pPort->getLibrary() + ".h\"\n";
		}

		headerContent += "\n";

		for( const InterfaceType* pInterface : device.getInterfaces() )
		{
			headerContent += "#include \""_s + pInterface->getFilename( "proxy", "h" ) + "\"\n";
		}

		headerContent += "\n";

		for( const InterfaceType* pInterface : device.getInterfaces() )
		{
			if( pInterface == pDeviceInterface )
			{
				continue;
			}
			else if( pInterface->getInternal() )
			{
				headerContent += "#include \""_s + pInterface->getHeaderFilename() + "\"\n";
			}
			else
			{
				headerContent += "#include \"../"_s + pInterface->getFilename( "impl", "h" ) + "\"\n";
			}
		}

		headerContent += "\n";
		headerContent += "namespace conct\n";
		headerContent += "{\n";
		headerContent += "\tclass DeviceInterface : public "_s + pDeviceInterface->getCppName() + "\n";
		headerContent += "\t{\n";
		headerContent += "\tpublic:\n";
		headerContent += "\n";
		headerContent += "\t\tDeviceInterface();\n";
		headerContent += "\n";
		headerContent += "\t\tvoid setupDevice();\n";
		headerContent += "\t\tvoid loopDevice();\n";
		headerContent += "\n";
		headerContent += "\t\tvirtual const char* getName() const CONCT_OVERRIDE_FINAL;\n";
		headerContent += "\n";
		headerContent += "\tprotected:\n";
		headerContent += "\n";
		headerContent += "\t\t"_s + runitmeClassName + " m_runtime;\n";
		headerContent += "\n";

		for( const DevicePort& port : device.getPorts() )
		{
			headerContent += "\t\t"_s + port.pPort->getClass() + " m_port" + string_tools::toString( port.index ) + ";\n";
		}

		headerContent += "\n";

		for( const InterfaceType* pInterface : device.getProxies() )
		{
			headerContent += "\t\t"_s + getTypeCppName( pInterface ) + "Proxy m_proxy" + pInterface->getName() + ";\n";
		}

		headerContent += "\n";

		for( const DeviceInstance& instance : device.getInstances() )
		{
			if( instance.pInterface == pDeviceInterface )
			{
				continue;
			}
			else if( instance.pInterface->getInternal() )
			{
				headerContent += "\t\t"_s + getTypeCppName( instance.pInterface ) + " m_instance" + instance.pInterface->getName() + ";\n";
			}
			else
			{
				headerContent += "\t\t"_s + getTypeCppName( instance.pInterface ) + "Impl m_instance" + instance.pInterface->getName() + ";\n";
			}
		}

		headerContent += "\n";
		headerContent += "\t\tvirtual void setup() = 0;\n";
		headerContent += "\t\tvirtual void loop() = 0;\n";
		headerContent += "\n";
		headerContent += "\t\tvirtual void getEmptyInstances( Array< Instance >& instances ) CONCT_OVERRIDE_FINAL;\n";
		headerContent += "\t\tvirtual void getPublicInstances( ArrayView< Instance >& instances ) const CONCT_OVERRIDE_FINAL;\n";
		headerContent += "\t\tvirtual void getLocalInstances( ArrayView< LocalInstance >& instances ) CONCT_OVERRIDE_FINAL;\n";
		headerContent += "\t};\n";
		headerContent += "}\n";

		DynamicString sourceContent;
		sourceContent += "#include \"device_interface.h\"\n";
		sourceContent += "\n";
		sourceContent += "namespace conct\n";
		sourceContent += "{\n";

		sourceContent += "\tDeviceInterface::DeviceInterface()\n";

		if( device.getHardware().getRuntime() == HardwareRuntime_High )
		{
			sourceContent += "\t\t: m_instanceRouter( m_runtime )\n";
		}

		sourceContent += "\t{\n";
		sourceContent += "\t}\n";
		sourceContent += "\n";

		sourceContent += "\tvoid DeviceInterface::setupDevice()\n";
		sourceContent += "\t{\n";

		for( const DevicePort& port : device.getPorts() )
		{
			sourceContent += "\t\tm_port"_s + string_tools::toString( port.index ) + ".setup();\n";
		}

		sourceContent += "\n";
		sourceContent += "\t\tm_runtime.setup( this );\n";

		if( runtime == HardwareRuntime_High )
		{
			for( const DevicePort& port : device.getPorts() )
			{
				sourceContent += "\t\tm_runtime.registerPort( &m_port"_s + string_tools::toString( port.index ) + " );\n";
			}
		}

		sourceContent += "\n";
		sourceContent += "\t\tsetup();\n";
		sourceContent += "\t}\n";
		sourceContent += "\n";

		sourceContent += "\tvoid DeviceInterface::loopDevice()\n";
		sourceContent += "\t{\n";

		for( const DevicePort& port : device.getPorts() )
		{
			sourceContent += "\t\tm_port"_s + string_tools::toString( port.index ) + ".loop();\n";
		}

		sourceContent += "\n";

		for( const DevicePort& port : device.getPorts() )
		{
			sourceContent += "\t\tm_runtime.processPort( &m_port"_s + string_tools::toString( port.index ) + " );\n";
		}

		sourceContent += "\n";
		sourceContent += "\t\tloop();\n";
		sourceContent += "\t}\n";
		sourceContent += "\n";

		sourceContent += "\tconst char* DeviceInterface::getName() const\n";
		sourceContent += "\t{\n";
		sourceContent += "\t\t return \""_s + device.getName() + "\";\n";
		sourceContent += "\t}\n";
		sourceContent += "\n";

		sourceContent += "\tvoid DeviceInterface::getEmptyInstances( Array< Instance >& instances )\n";
		sourceContent += "\t{\n";

		if( device.getInstances().isEmpty() )
		{
			sourceContent += "\t\tinstances.set( nullptr, 0u );\n";
		}
		else
		{
			sourceContent += "\t\tstatic Instance s_instances[ "_s + string_tools::toString( device.getInstances().getLength() ) + "u ];\n";
			sourceContent += "\t\tinstances.set( s_instances, CONCT_COUNT( s_instances ) );\n";
		}

		sourceContent += "\t}\n";
		sourceContent += "\n";

		sourceContent += "\tvoid DeviceInterface::getPublicInstances( ArrayView< Instance >& instances ) const\n";
		sourceContent += "\t{\n";

		if( device.getInstances().isEmpty() )
		{
			sourceContent += "\t\tinstances.set( nullptr, 0u );\n";
		}
		else
		{
			sourceContent += "\t\tstatic const Instance s_instances[] =\n";
			sourceContent += "\t\t{\n";

			for( const DeviceInstance& instance : device.getInstances() )
			{
				sourceContent += "\t\t\t{ "_s + string_tools::toString( instance.id ) + ", " + string_tools::toString( instance.pInterface->getCrc() ) + " },\n";
			}

			sourceContent += "\t\t};\n";
			sourceContent += "\n";
			sourceContent += "\t\tinstances.set( s_instances, CONCT_COUNT( s_instances ) );\n";
		}

		sourceContent += "\t}\n";
		sourceContent += "\n";

		sourceContent += "\tvoid DeviceInterface::getLocalInstances( ArrayView< LocalInstance >& instances )\n";
		sourceContent += "\t{\n";

		if( device.getInstances().isEmpty() )
		{
			sourceContent += "\t\tinstances.set( nullptr, 0u );\n";
		}
		else
		{
			sourceContent += "\t\tstatic const LocalInstance s_instances[] =\n";
			sourceContent += "\t\t{\n";

			for( const DeviceInstance& instance : device.getInstances() )
			{
				if( instance.pInterface == pDeviceInterface )
				{
					sourceContent += "\t\t\t{ "_s + string_tools::toString( instance.id ) + ", this, &m_proxy" + instance.pInterface->getName() + " },\n";
				}
				else
				{
					sourceContent += "\t\t\t{ "_s + string_tools::toString( instance.id ) + ", &m_instance" + instance.pInterface->getName() + ", &m_proxy" + instance.pInterface->getName() + " },\n";
				}
			}

			sourceContent += "\t\t};\n";
			sourceContent += "\n";
			sourceContent += "\t\tinstances.set( s_instances, CONCT_COUNT( s_instances ) );\n";
		}

		sourceContent += "\t}\n";
		sourceContent += "}\n";

		const Path headerFilepath = parameters.genOutputPath.push( "device_interface.h"_s );
		const Path sourceFilepath = parameters.genOutputPath.push( "device_interface.cpp"_s );
		return writeStringToFile( headerFilepath, headerContent, false ) &&
			writeStringToFile( sourceFilepath, sourceContent, false );
	}

	bool Generator::exportDeviceImpl( const Device& device, const GeneratorParameters& parameters )
	{
		const DynamicString deviceClassName = "Device"_s + device.getName();

		DynamicString headerContent;
		headerContent += "#pragma once\n";
		headerContent += "\n";
		headerContent += "#include \"gen/device_interface.h\"\n";
		headerContent += "\n";
		headerContent += "namespace conct\n";
		headerContent += "{\n";
		headerContent += "\tclass "_s + deviceClassName + " : public DeviceInterface\n";
		headerContent += "\t{\n";
		headerContent += "\tprotected:\n";
		headerContent += "\n";
		headerContent += "\t\tvirtual void setup() CONCT_OVERRIDE_FINAL;\n";
		headerContent += "\t\tvirtual void loop() CONCT_OVERRIDE_FINAL;\n";
		headerContent += "\t};\n";
		headerContent += "}\n";

		DynamicString sourceContent;
		sourceContent += "#include \"device_impl.h\"\n";
		sourceContent += "\n";
		sourceContent += "namespace conct\n";
		sourceContent += "{\n";
		sourceContent += "\tvoid "_s + deviceClassName + "::setup()\n";
		sourceContent += "\t{\n";
		sourceContent += "\t\t// TODO: implement\n";
		sourceContent += "\t}\n";
		sourceContent += "\n";
		sourceContent += "\tvoid "_s + deviceClassName + "::loop()\n";
		sourceContent += "\t{\n";
		sourceContent += "\t\t// TODO: implement\n";
		sourceContent += "\t}\n";
		sourceContent += "}\n";

		const Path headerFilepath = parameters.outputPath.push( "device_impl.h"_s );
		const Path sourceFilepath = parameters.outputPath.push( "device_impl.cpp"_s );
		return writeStringToFile( headerFilepath, headerContent, true ) &&
			writeStringToFile( sourceFilepath, sourceContent, true );
	}

	bool Generator::exportIno( const Device& device, const GeneratorParameters& parameters )
	{
		if( device.getHardware().getRuntime() == HardwareRuntime_High )
		{
			return true;
		}

		const DynamicString deviceClassName = "Device"_s + device.getName();

		DynamicString headerContent;
		headerContent += "#pragma once\n";
		headerContent += "\n";

		for( const Path& file : m_exportedFiles )
		{
			if( file.getParent() != parameters.genOutputPath ||
				file.getExtension() != ".cpp" )
			{
				continue;
			}

			headerContent += "#include \""_s + file.getFilename() + "\"\n";
		}

		headerContent += "\n";

		DynamicString sourceContent;
		sourceContent += "#include \"device_impl.h\"\n";
		sourceContent += "#include \"gen/files.h\"\n";
		sourceContent += "\n";
		sourceContent += "conct::"_s + deviceClassName + " device;\n";
		sourceContent += "\n";
		sourceContent += "void setup()\n";
		sourceContent += "{\n";
		sourceContent += "\tdevice.setupDevice();\n";
		sourceContent += "}\n";
		sourceContent += "\n";
		sourceContent += "void loop()\n";
		sourceContent += "{\n";
		sourceContent += "\tdevice.loopDevice();\n";
		sourceContent += "}\n";
		sourceContent += "\n";

		const DynamicString sourceFilename = getDeviceFilename( device );
		if( parameters.outputPath.getFilename() != sourceFilename )
		{
			trace::write( "Error: device name("_s + sourceFilename + ") is not equals to the device folder name(" + parameters.outputPath.getFilename() + ").\n" );
			return false;
		}

		const Path sourceFilepath = parameters.outputPath.push( sourceFilename + ".ino" );
		const Path headerFilepath = parameters.genOutputPath.push( "files.h"_s );
		return writeStringToFile( headerFilepath, headerContent, false ) &&
			writeStringToFile( sourceFilepath, sourceContent, true );
	}

	void Generator::writeDependingTypeIncludes( DynamicString& target, const Type* pType )
	{
		UnsortedSet< DynamicString > headerFiles;

		for( const Type* pDependingType : pType->getDependingTypes() )
		{
			headerFiles.insert( pDependingType->getHeaderFilename() );
		}

		for( const DynamicString& headerFile : headerFiles )
		{
			target += "#include \""_s + headerFile + "\"\n";
		}
	}
}
