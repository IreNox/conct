#include "generator.h"

#include "device.h"
#include "hardware.h"
#include "port.h"

#include "conct_array_type.h"
#include "conct_crc16.h"
#include "conct_enum_type.h"
#include "conct_filesystem.h"
#include "conct_interface_type.h"
#include "conct_struct_type.h"
#include "conct_trace.h"

#include <tiki/tiki_string.h>

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

	bool Generator::run( const DynamicArray< DynamicString >& arguments )
	{
		const Path exePath = filesystem::getExecutablePath();
		const Path basePath = exePath.getParent().getParent().getParent().getParent().getParent().getParent();
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
			!exportDeviceImpl( device, parameters ) )
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
		if( pType->getKind() == TypeKind::Array )
		{
			const ArrayType* pArrayType = static_cast< const ArrayType* >( pType );
			return "ArrayView< "_s + getTypeCppReturnName( pArrayType->getBaseType() ) + " >";
		}
		//else if( pType->getKind() == TypeKind::Interface )
		//{
		//	//return
		//}

		return getTypeCppName( pType );
	}

	DynamicString Generator::getTypeCppArgumentName( const Type* pType ) const
	{
		return getTypeCppName( pType );
	}

	DynamicString Generator::getTypeCppValueGetFunction( const Type* pType, const DynamicString& variableName ) const
	{
		if( pType->getKind() == TypeKind::Enum )
		{
			return "("_s + pType->getCppName() + ")(" + variableName + ".getInteger() )";
		}

		return variableName + ".get" + pType->getName() + "()";
	}

	DynamicString Generator::getTypeCppValueSetFunction( const Type* pType, const DynamicString& variableName, const DynamicString& value ) const
	{
		if( pType->getKind() == TypeKind::Array )
		{
			return variableName + ".setArray( " + value + " )";
		}
		else if( pType->getKind() == TypeKind::Struct )
		{
			return variableName + ".setStruct( " + value + " )";
		}
		else if( pType->getValueType() == ValueType::Void )
		{
			return value + "; " + variableName + ".setVoid()";
		}

		return variableName + ".set" + pType->getName() + "( " + value + " )";
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
					filename = filename.pushBack( '_' );
				}

				filename = filename.pushBack( ascii::toLower( c ) );
			}
			else
			{
				filename = filename.pushBack( c );
			}
		}

		return filename;
	}

	DynamicString Generator::getStringHexCrc16( const DynamicString& string ) const
	{
		return "0x"_s + string_tools::toHexString( calculateStringCrc16( string.toConstCharPointer() ) );
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

	bool Generator::parseArguments( GeneratorParameters& parameters, const DynamicArray< DynamicString >& arguments )
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
		if( pType->getKind() == TypeKind::Value )
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
			if( pType->getKind() == TypeKind::Interface )
			{
				const InterfaceType* pInterface = static_cast<const InterfaceType*>(pType);
				if( !exportInterface( device, parameters, pInterface ) )
				{
					return false;
				}
			}
			else if( pType->getKind() == TypeKind::Struct )
			{
				const StructType* pStruct = static_cast< const StructType* >( pType );
				if( !exportStruct( device, parameters, pStruct ) )
				{
					return false;
				}
			}
			else if( pType->getKind() == TypeKind::Enum )
			{
				const EnumType* pEnum = static_cast<const EnumType*>(pType);
				if( !exportEnum( device, parameters, pEnum ) )
				{
					return false;
				}
			}
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

	bool Generator::exportStruct( const Device& device, const GeneratorParameters& parameters, const StructType* pStruct )
	{
		if( pStruct->getInternal() )
		{
			return true;
		}

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

	bool Generator::exportEnum( const Device& device, const GeneratorParameters& parameters, const EnumType* pEnum )
	{
		if( pEnum->getInternal() )
		{
			return true;
		}

		DynamicString headerContent;
		headerContent += "#pragma once\n";
		headerContent += "\n";
		headerContent += "namespace conct\n";
		headerContent += "{\n";
		headerContent += "\tstatic constexpr TypeCrc "_s + pEnum->getName() + "TypeCrc = " + string_tools::toString( pEnum->getCrc() ) + ";\n";
		headerContent += "\n";
		headerContent += "\tenum "_s + pEnum->getName() + "\n";
		headerContent += "\t{\n";

		for( const EnumValue& value : pEnum->getValues() )
		{
			headerContent += "\t\t"_s + pEnum->getName() + "_" + value.name + ",\n";
		}

		headerContent += "\t};\n";
		headerContent += "}\n";

		const Path headerFilepath = parameters.genOutputPath.push( pEnum->getHeaderFilename() );
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
			headerContent += "\t\tvirtual bool getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, uint16 nameCrc ) const TIKI_OVERRIDE_FINAL;\n";
			headerContent += "\t\tvirtual bool setProperty( void* pInstance, uint16 nameCrc, const Value& value ) const TIKI_OVERRIDE_FINAL;\n";
			headerContent += "\n";
			headerContent += "\t\tvirtual bool callFunction( ValueBuilder& targetValueBuilder, void* pInstance, uint16 nameCrc, const ArrayView< const Value >& parameters ) const TIKI_OVERRIDE_FINAL;\n";
			headerContent += "\t};\n";
			headerContent += "}\n";

			DynamicString sourceContent;
			sourceContent += "#include \""_s + headerFilename + "\"\n";
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

			sourceContent += "\tbool "_s + proxyName + "::getProperty( ValueBuilder& targetValueBuilder, const void* pInstance, uint16 nameCrc ) const\n";
			sourceContent += "\t{\n";

			bool isFirst = true;
			for( const InterfaceProperty& property : pInterface->getProperties() )
			{
				if( !property.hasGetter )
				{
					continue;
				}

				if( isFirst )
				{
					sourceContent += "\t\tconst "_s + pInterface->getName() + "* pTypedInstance = static_cast< const " + pInterface->getName() + "* >( pInstance );\n";
					sourceContent += "\n";

					isFirst = false;
				}

				sourceContent += "\t\tif( nameCrc == "_s + getStringHexCrc16( property.name ) + " )\n";
				sourceContent += "\t\t{\n";
				sourceContent += "\t\t\t"_s + getTypeCppValueSetFunction( property.pType, "targetValueBuilder"_s, "pTypedInstance->get"_s + property.name + "()" ) + ";\n";
				sourceContent += "\t\t\treturn true;\n";
				sourceContent += "\t\t}\n";
				sourceContent += "\n";
			}

			sourceContent += "\t\treturn false;\n";
			sourceContent += "\t}\n";
			sourceContent += "\n";

			sourceContent += "\tbool "_s + proxyName + "::setProperty( void* pInstance, uint16 nameCrc, const Value& value ) const\n";
			sourceContent += "\t{\n";

			isFirst = true;
			for( const InterfaceProperty& property : pInterface->getProperties() )
			{
				if( !property.hasSetter )
				{
					continue;
				}

				if( isFirst )
				{
					sourceContent += "\t\t"_s + pInterface->getName() + "* pTypedInstance = static_cast< " + pInterface->getName() + "* >( pInstance );\n";
					sourceContent += "\n";

					isFirst = false;
				}

				sourceContent += "\t\tif( nameCrc == "_s + getStringHexCrc16( property.name ) + " )\n";
				sourceContent += "\t\t{\n";
				sourceContent += "\t\t\tpTypedInstance->set"_s + property.name + "( value.get" +  property.pType->getName() + "() );\n";
				sourceContent += "\t\t\treturn true;\n";
				sourceContent += "\t\t}\n";
				sourceContent += "\n";
			}

			sourceContent += "\t\treturn false;\n";
			sourceContent += "\t}\n";
			sourceContent += "\n";

			sourceContent += "\tbool "_s + pInterface->getName() + "Proxy::callFunction( ValueBuilder& targetValueBuilder, void* pInstance, uint16 nameCrc, const ArrayView< const Value >& parameters ) const\n";
			sourceContent += "\t{\n";

			if( !pInterface->getFunctions().isEmpty() )
			{
				sourceContent += "\t\t"_s + pInterface->getName() + "* pTypedInstance = static_cast< " + pInterface->getName() + "* >( pInstance );\n";
				sourceContent += "\n";
			}

			for( const InterfaceFunction& function : pInterface->getFunctions() )
			{
				sourceContent += "\t\tif( nameCrc == "_s + getStringHexCrc16( function.name ) + " )\n";
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

					parameters += getTypeCppValueGetFunction( parameter.pType, "parameters[ "_s + string_tools::toString( i ) + "u ]" );
					i++;
				}

				if( !function.parameters.isEmpty() )
				{
					parameters += " ";
				}

				sourceContent += "\t\t\t"_s + getTypeCppValueSetFunction( function.pReturnType, "targetValueBuilder"_s, "pTypedInstance->"_s + function.name + "(" + parameters + ")" ) + ";\n";
				sourceContent += "\t\t\treturn true;\n";
				sourceContent += "\t\t}\n";
				sourceContent += "\n";
			}

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
					headerContent += "\t\tvirtual "_s + getTypeCppReturnName( property.pType ) + " get" + property.name + "() const TIKI_OVERRIDE_FINAL;\n";
				}

				if( property.hasSetter )
				{
					headerContent += "\t\tvirtual void set"_s + property.name + "( " + getTypeCppArgumentName( property.pType ) + " value ) TIKI_OVERRIDE_FINAL;\n";
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

				headerContent += "\t\tvirtual "_s + getTypeCppReturnName( function.pReturnType ) + " " + function.name + "( " + parameters + " ) TIKI_OVERRIDE_FINAL;\n";
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

		DynamicString headerContent;
		headerContent += "#pragma once\n";
		headerContent += "\n";
		headerContent += "#include \""_s + pDeviceInterface->getHeaderFilename() + "\"\n";
		headerContent += "#include \"conct_runtime.h\"\n";
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
		headerContent += "\t\tvirtual const char* getName() const TIKI_OVERRIDE_FINAL;\n";
		headerContent += "\n";
		headerContent += "\tprotected:\n";
		headerContent += "\n";
		headerContent += "\t\tRuntime m_runtime;\n";
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
		headerContent += "\t\tvirtual void getEmptyInstances( ArrayView< Instance >& instances ) TIKI_OVERRIDE_FINAL;\n";
		headerContent += "\t\tvirtual void getPublicInstances( ConstInstanceView& instances ) const TIKI_OVERRIDE_FINAL;\n";
		headerContent += "\t\tvirtual void getLocalInstances( ArrayView< const LocalInstance >& instances ) TIKI_OVERRIDE_FINAL;\n";
		headerContent += "\t};\n";
		headerContent += "}\n";

		DynamicString sourceContent;
		sourceContent += "#include \"device_interface.h\"\n";
		sourceContent += "\n";
		sourceContent += "namespace conct\n";
		sourceContent += "{\n";

		sourceContent += "\tDeviceInterface::DeviceInterface()\n";

		if( device.needsRouter() )
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
			const DynamicString portIndexString = string_tools::toString( port.index );

			const Port::ParameterMap& portParameters = port.pPort->getParameters();
			if( !portParameters.isEmpty() )
			{
				const DynamicString parametersVariableName = "port"_s + portIndexString + "Parameters";
				sourceContent += "\t\t"_s + port.pPort->getParametersStruct() + " " + parametersVariableName + ";\n";

				for( const Port::ParameterMap::PairType& kvp : portParameters )
				{
					const PortParameter* pParameter = kvp.value;
					if( !pParameter->matchFilters( device ) )
					{
						continue;
					}

					const PortParameterValue* pValue = port.parameters.find( pParameter );
					if( pValue == nullptr )
					{
						pValue = &pParameter->getDefaultValue();
					}

					switch( pValue->type )
					{
					case PortParameterValueType::Invalid:
						sourceContent += "error";
						break;

					case PortParameterValueType::String:
						sourceContent += "\t\t"_s + parametersVariableName + "." + pParameter->getName() + " = \"" + pValue->string + "\"_s;\n";
						break;

					case PortParameterValueType::Integer:
						sourceContent += "\t\t"_s + parametersVariableName + "." + pParameter->getName() + " = " + string_tools::toString( pValue->integer ) + ";\n";
						break;
					}
				}

				sourceContent += "\t\tm_port"_s + portIndexString + ".setup( " + parametersVariableName + " );\n";
			}
			else
			{
				sourceContent += "\t\tm_port"_s + portIndexString + ".setup();\n";
			}
		}

		sourceContent += "\n";
		sourceContent += "\t\tm_runtime.setup( this );\n";

		for( const DevicePort& port : device.getPorts() )
		{
			sourceContent += "\t\tm_runtime.registerPort( &m_port"_s + string_tools::toString( port.index ) + " );\n";
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

		sourceContent += "\tvoid DeviceInterface::getEmptyInstances( ArrayView< Instance >& instances )\n";
		sourceContent += "\t{\n";

		if( device.getInstances().isEmpty() )
		{
			sourceContent += "\t\tinstances.set( nullptr, 0u );\n";
		}
		else
		{
			sourceContent += "\t\tstatic Instance s_instances[ "_s + string_tools::toString( device.getInstances().getLength() ) + "u ];\n";
			sourceContent += "\t\tinstances = ArrayView< Instance >( s_instances, TIKI_ARRAY_COUNT( s_instances ) );\n";
		}

		sourceContent += "\t}\n";
		sourceContent += "\n";

		sourceContent += "\tvoid DeviceInterface::getPublicInstances( ConstInstanceView& instances ) const\n";
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
			sourceContent += "\t\tinstances.set( s_instances, TIKI_ARRAY_COUNT( s_instances ) );\n";
		}

		sourceContent += "\t}\n";
		sourceContent += "\n";

		sourceContent += "\tvoid DeviceInterface::getLocalInstances( ArrayView< const LocalInstance >& instances )\n";
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
			sourceContent += "\t\tinstances.set( s_instances, TIKI_ARRAY_COUNT( s_instances ) );\n";
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

		const DynamicString deviceFilename = getDeviceFilename( device );
		const DynamicString headerFilename = "device_"_s + deviceFilename + ".h";

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
		headerContent += "\t\tvirtual void setup() TIKI_OVERRIDE_FINAL;\n";
		headerContent += "\t\tvirtual void loop() TIKI_OVERRIDE_FINAL;\n";
		headerContent += "\t};\n";
		headerContent += "}\n";

		DynamicString sourceContent;
		sourceContent += "#include \""_s + headerFilename + "\"\n";
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

		const Path headerFilepath = parameters.outputPath.push( headerFilename );
		const Path sourceFilepath = parameters.outputPath.push( "device_"_s + deviceFilename + ".cpp" );
		return writeStringToFile( headerFilepath, headerContent, true ) &&
			writeStringToFile( sourceFilepath, sourceContent, true );
	}

	void Generator::writeDependingTypeIncludes( DynamicString& target, const Type* pType )
	{
		HashSet< DynamicString > headerFiles;

		for( const Type* pDependingType : pType->getDependingTypes() )
		{
			headerFiles.insert( pDependingType->getHeaderFilename() );
		}

		for( const DynamicString& headerFile : headerFiles )
		{
			if( headerFile.startsWith( "tiki/" ) )
			{
				target += "#include <"_s + headerFile + ">\n";
			}
			else
			{
				target += "#include \""_s + headerFile + "\"\n";
			}
		}
	}
}
