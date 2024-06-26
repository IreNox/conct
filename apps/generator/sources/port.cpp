#include "port.h"

#include "conct_xml_helper.h"
#include "conct_trace.h"

#include "port_parameter.h"

#include <tinyxml2/tinyxml2.h>

namespace conct
{
	Port::Port()
	{
		m_mtu				= 0u;
		m_singleConnection	= false;
		m_multiConnection	= false;
		m_reliable			= false;
	}

	Port::~Port()
	{
		for( const ParameterMap::PairType& kvp : m_parameters )
		{
			delete kvp.value;
		}
	}

	bool Port::load( const Path& filename )
	{
		tinyxml2::XMLDocument document;
		if( !loadDocument( document, filename ) )
		{
			return false;
		}

		const tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "port" );
		if( pRootNode == nullptr )
		{
			trace::write( "Error: Failed to find 'port' root node in '"_s + filename.getGenericPath() + "'." + "\n" );
			return false;
		}

		if( !loadStringValue( m_name, pRootNode, "name" ) ||
			!loadStringValue( m_library, pRootNode, "library" ) ||
			!loadStringValue( m_class, pRootNode, "class" ) )
		{
			return false;
		}

		const tinyxml2::XMLElement* pConnectionNode = pRootNode->FirstChildElement( "connection" );
		if( pConnectionNode == nullptr )
		{
			trace::write( "Error: Failed to find 'connection' root node in '"_s + filename.getGenericPath() + "'." + "\n" );
			return false;
		}

		const tinyxml2::XMLElement* pParametersNode = pRootNode->FirstChildElement( "parameters" );
		if( pParametersNode != nullptr )
		{
			if( !loadStringValue( m_parametersStruct, pRootNode, "parameters_struct" ) )
			{
				return false;
			}

			for( const tinyxml2::XMLElement* pParameterNode = pParametersNode->FirstChildElement( "parameter" ); pParameterNode != nullptr; pParameterNode = pParameterNode->NextSiblingElement( "parameter" ) )
			{
				PortParameter* pParameter = new PortParameter();
				if( !pParameter->load( pParameterNode ) )
				{
					trace::write( "Error: Failed to load parameter.\n" );
					delete pParameter;
					return false;
				}

				m_parameters[ pParameter->getName() ] = pParameter;
			}
		}

		if( !loadMemSizeValue( m_mtu, pConnectionNode, "mtu" ) ||
			!loadBooleanValue( m_singleConnection, pConnectionNode, "single_device" ) ||
			!loadBooleanValue( m_multiConnection, pConnectionNode, "multi_device" ) ||
			!loadBooleanValue( m_reliable, pConnectionNode, "reliable" ) )
		{
			return false;
		}

		const tinyxml2::XMLElement* pRequirementsNode = pRootNode->FirstChildElement( "requirements" );
		if( pRequirementsNode == nullptr )
		{
			trace::write( "Error: Failed to find 'requirements' root node in '"_s + filename.getGenericPath() + "'." + "\n" );
			return false;
		}

		return m_requirements.load( pRequirementsNode );
	}
}
