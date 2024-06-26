#pragma once

#include "hardware_collection.h"
#include "port_collection.h"

#include "conct_path.h"
#include "conct_type_collection.h"
#include "conct_unsorted_set.h"

namespace conct
{
	class Device;
	class Path;
	struct GeneratorParameters;

	class Generator
	{
	public:

				Generator();

		bool	run( const Vector< DynamicString >& arguments );

	private:

		typedef UnsortedSet< const Type* > TypeSet;
		typedef UnsortedSet< Path > PathSet;

		HardwareCollection	m_hardware;
		PortCollection		m_ports;
		TypeCollection		m_types;

		PathSet				m_exportedFiles;

		DynamicString		getTypeCppName( const Type* pType ) const;
		DynamicString		getTypeCppReturnName( const Type* pType ) const;
		DynamicString		getTypeCppArgumentName( const Type* pType ) const;
		DynamicString		getTypeCppValueGetFunction( const Type* pType, const DynamicString& variableName ) const;
		DynamicString		getTypeCppValueSetFunction( const Type* pType, const DynamicString& variableName, const DynamicString& value ) const;
		DynamicString		getDeviceFilename( const Device& device ) const;
		DynamicString		getStringHexCrc16( const DynamicString& string ) const;

		bool				writeStringToFile( const Path& filename, const DynamicString& content, bool ifNotExists );

		bool				parseArguments( GeneratorParameters& parameters, const Vector< DynamicString >& arguments );
		void				collectTypesToExport( TypeSet& types, const Device& device );
		void				collectTypesToExport( TypeSet& types, const Type* pType );

		bool				exportTypes( const Device& device, const GeneratorParameters& parameters );
		bool				exportInterface( const Device& device, const GeneratorParameters& parameters, const InterfaceType* pInterface );
		bool				exportStruct( const Device& device, const GeneratorParameters& parameters, const StructType* pStruct );
		bool				exportEnum( const Device& device, const GeneratorParameters& parameters, const EnumType* pEnum );

		bool				exportInterfaceProxies( const Device& device, const GeneratorParameters& parameters );
		bool				exportInterfaceImpl( const Device& device, const GeneratorParameters& parameters );
		bool				exportDeviceInterface( const Device& device, const GeneratorParameters& parameters );
		bool				exportDeviceImpl( const Device& device, const GeneratorParameters& parameters );
		bool				exportIno( const Device& device, const GeneratorParameters& parameters );

		void				writeDependingTypeIncludes( DynamicString& target, const Type* pType );
	};
}