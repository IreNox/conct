#include "hardware.h"

#include "service_collection.h"

#include "third_party/tinyxml2.h"

#include <iostream>

namespace conct
{
	static bool loadFromChildNode( size_t& target, tinyxml2::XMLElement* pNode, const char* pChildName )
	{
		tinyxml2::XMLElement* pChildNode = pNode->FirstChildElement( pChildName );
		if( pChildNode == nullptr )
		{
			std::cout << "Error: Failed to find '" << pChildNode << "' node in '" << pNode->Name() << "'." << std::endl;
			return false;
		}

		target = ( size_t )pChildNode->Int64Text();
		return true;
	}

	Hardware::Hardware()
	{
	}

	bool Hardware::load( const std::string& fileName, ServiceCollection& serviceCollection )
	{
		tinyxml2::XMLDocument document;
		if( document.LoadFile( fileName.c_str() ) != tinyxml2::XML_SUCCESS )
		{
			std::cout << "Error: Failed to load XML from '" << fileName << "'. Message: " << document.ErrorStr() << std::endl;
			return false;
		}

		tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "hardware" );
		if( pRootNode == nullptr )
		{
			std::cout << "Error: Failed to find 'hardware' root node in '" << fileName << "'." << std::endl;
			return false;
		}

		tinyxml2::XMLElement* pNameNode = pRootNode->FirstChildElement( "name" );
		if( pNameNode == nullptr )
		{
			std::cout << "Error: Failed to find 'name' node in '" << fileName << "'." << std::endl;
			return false;
		}

		m_name = pNameNode->Value();

		tinyxml2::XMLElement* pSpecsNode = pRootNode->FirstChildElement( "specifications" );
		if( pSpecsNode == nullptr )
		{
			std::cout << "Error: Failed to find 'specifications' node in '" << fileName << "'." << std::endl;
			return false;
		}

		if( !loadFromChildNode( m_programDataSize, pSpecsNode, "program" ) ||
			!loadFromChildNode( m_ramSize, pSpecsNode, "ram" ) ||
			!loadFromChildNode( m_flashSize, pSpecsNode, "flash" ) )
		{
			return false;
		}

		tinyxml2::XMLElement* pServicesNode = pRootNode->FirstChildElement( "services" );
		if( pSpecsNode == nullptr )
		{
			std::cout << "Error: Failed to find 'services' node in '" << fileName << "'." << std::endl;
			return false;
		}

		tinyxml2::XMLElement* pServiceNode = pServicesNode->FirstChildElement();
		while( pServiceNode != nullptr )
		{
			const Service* pService = serviceCollection.findServiceByTag( pServiceNode->Name() );
			if( pService == nullptr )
			{
				std::cout << "Error: Failed to find service with name '" << pServiceNode->Name() << "'." << std::endl;
				return false;
			}

			m_services.push_back( pService );

			pServiceNode = pServiceNode->NextSiblingElement();
		}

		return true;
	}
}
