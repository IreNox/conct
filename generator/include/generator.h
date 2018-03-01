#pragma once

#include "base.h"
#include "hardware_collection.h"
#include "port_collection.h"
#include "type_collection.h"

namespace conct
{
	class Device;
	struct GeneratorParameters;

	class Generator
	{
	public:

				Generator();

		bool	run( const std::vector< std::string >& arguments );

	private:

		HardwareCollection	m_hardware;
		PortCollection		m_ports;
		TypeCollection		m_types;

		void				registerTypes();

		std::string			combinePath( const std::string& path1, const std::string& path2 );
		std::string			replaceString( const std::string& text, const std::string& from, const std::string& to ) const;

		std::string			getTypeFilename( const Type* pType, const std::string& appendix, const std::string& extension ) const;
		std::string			getTypeCppName( const Type* pType ) const;
		std::string			getTypeCppReturnName( const Type* pType ) const;
		std::string			getTypeCppArgumentName( const Type* pType ) const;

		bool				writeStringToFile( const std::string& filename, const std::string& content ) const;

		bool				parseArguments( GeneratorParameters& parameters, const std::vector< std::string >& arguments );
		bool				exportInterfaces( const Device& device, const GeneratorParameters& parameters );
		bool				exportProxies( const Device& device, const GeneratorParameters& parameters );
		bool				exportDeviceClass( const Device& device, const GeneratorParameters& parameters );
	};
}