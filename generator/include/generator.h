#pragma once

#include "base.h"
#include "hardware_collection.h"
#include "port_collection.h"
#include "type_collection.h"

#include <set>

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

		typedef std::set< std::filesystem::path > PathSet;

		HardwareCollection	m_hardware;
		PortCollection		m_ports;
		TypeCollection		m_types;

		PathSet				m_exportedFiles;

		void				registerTypes();

		std::string			replaceString( const std::string& text, const std::string& from, const std::string& to ) const;

		std::string			getTypeFilename( const Type* pType, const std::string& appendix, const std::string& extension ) const;
		std::string			getTypeCppName( const Type* pType ) const;
		std::string			getTypeCppReturnName( const Type* pType ) const;
		std::string			getTypeCppArgumentName( const Type* pType ) const;
		std::string			getDeviceFilename( const Device& device ) const;

		bool				writeStringToFile( const std::filesystem::path& filename, const std::string& content, bool ifNotExists );

		bool				parseArguments( GeneratorParameters& parameters, const std::vector< std::string >& arguments );
		bool				exportInterfaces( const Device& device, const GeneratorParameters& parameters );
		bool				exportInterfaceProxies( const Device& device, const GeneratorParameters& parameters );
		bool				exportInterfaceImpl( const Device& device, const GeneratorParameters& parameters );
		bool				exportDeviceInterface( const Device& device, const GeneratorParameters& parameters );
		bool				exportDeviceImpl( const Device& device, const GeneratorParameters& parameters );
		bool				exportIno( const Device& device, const GeneratorParameters& parameters );
	};
}