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

		typedef std::set< const Type* > TypeSet;
		typedef std::set< std::filesystem::path > PathSet;

		HardwareCollection	m_hardware;
		PortCollection		m_ports;
		TypeCollection		m_types;

		PathSet				m_exportedFiles;

		std::string			replaceString( const std::string& text, const std::string& from, const std::string& to ) const;

		std::string			getTypeCppName( const Type* pType ) const;
		std::string			getTypeCppReturnName( const Type* pType ) const;
		std::string			getTypeCppArgumentName( const Type* pType ) const;
		std::string			getTypeCppValueSetName( const Type* pType ) const;
		std::string			getDeviceFilename( const Device& device ) const;

		bool				writeStringToFile( const std::filesystem::path& filename, const std::string& content, bool ifNotExists );

		bool				parseArguments( GeneratorParameters& parameters, const std::vector< std::string >& arguments );
		void				collectTypesToExport( TypeSet& types, const Device& device );
		void				collectTypesToExport( TypeSet& types, const Type* pType );

		bool				exportTypes( const Device& device, const GeneratorParameters& parameters );
		bool				exportStruct( const Device& device, const GeneratorParameters& parameters, const StructType* pStruct );
		bool				exportInterface( const Device& device, const GeneratorParameters& parameters, const InterfaceType* pInterface );

		bool				exportInterfaceProxies( const Device& device, const GeneratorParameters& parameters );
		bool				exportInterfaceImpl( const Device& device, const GeneratorParameters& parameters );
		bool				exportDeviceInterface( const Device& device, const GeneratorParameters& parameters );
		bool				exportDeviceImpl( const Device& device, const GeneratorParameters& parameters );
		bool				exportIno( const Device& device, const GeneratorParameters& parameters );

		void				writeDependingTypeIncludes( std::ostringstream& target, const Type* pType );
	};
}