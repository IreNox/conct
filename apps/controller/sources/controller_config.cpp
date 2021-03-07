#include "controller_config.h"

#include "conct_string_tools.h"
#include "conct_trace.h"

#include "tinyxml2/tinyxml2.h"

namespace conct
{
	void ControllerConfig::load()
	{
		tinyxml2::XMLDocument xmlDocument;
		if( xmlDocument.LoadFile( "config.xml" ) != tinyxml2::XML_SUCCESS )
		{
			trace::write( "Failed to load config!" );
			return;
		}

		tinyxml2::XMLElement* pRootNode		= xmlDocument.RootElement();
		tinyxml2::XMLNode* pConnectionsNode	= pRootNode->FirstChildElement( "connections" );
		if( pConnectionsNode == nullptr )
		{
			trace::write( "Invalid config data!" );
			return;
		}

		for( tinyxml2::XMLElement* pConnectionNode = pConnectionsNode->FirstChildElement( "connection" ); pConnectionNode != nullptr; pConnectionNode = pConnectionNode->NextSiblingElement( "connection" ) )
		{
			tinyxml2::XMLElement* pTitleNode	= pConnectionNode->FirstChildElement( "title" );
			tinyxml2::XMLElement* pHostnameNode	= pConnectionNode->FirstChildElement( "hostname" );
			tinyxml2::XMLElement* pPortNode		= pConnectionNode->FirstChildElement( "port" );

			if( pTitleNode == nullptr ||
				pHostnameNode == nullptr ||
				pPortNode == nullptr )
			{
				trace::write( "Invalid config data!" );
				return;
			}

			Connection connection;
			connection.title.assign( pTitleNode->GetText() );
			connection.hostname.assign( pHostnameNode->GetText() );

			int64_t port;
			if( pPortNode->QueryInt64Text( &port ) != tinyxml2::XML_SUCCESS )
			{
				trace::write( "Invalid config data!" );
				return;
			}
			connection.port = (uint16)port;

			m_connections.pushBack( connection );
		}
	}

	void ControllerConfig::save()
	{
		tinyxml2::XMLDocument xmlDocument;

		tinyxml2::XMLElement* pRootNode = xmlDocument.NewElement( "controller_config" );
		xmlDocument.InsertEndChild( pRootNode );

		tinyxml2::XMLElement* pConnectionsNode = xmlDocument.NewElement( "connections" );
		pRootNode->InsertEndChild( pConnectionsNode );

		for( const Connection& connection : m_connections )
		{
			tinyxml2::XMLElement* pConnectionNode = xmlDocument.NewElement( "connection" );
			pConnectionsNode->InsertEndChild( pConnectionNode );

			tinyxml2::XMLElement* pTitleNode = xmlDocument.NewElement( "title" );
			pTitleNode->SetText( connection.title.toConstCharPointer() );
			pConnectionNode->InsertEndChild( pTitleNode );

			tinyxml2::XMLElement* pHostnameNode = xmlDocument.NewElement( "hostname" );
			pHostnameNode->SetText( connection.hostname.toConstCharPointer() );
			pConnectionNode->InsertEndChild( pHostnameNode );

			tinyxml2::XMLElement* pPortNode = xmlDocument.NewElement( "port" );
			pPortNode->SetText( string_tools::toString( connection.port ).toConstCharPointer() );
			pConnectionNode->InsertEndChild( pPortNode );
		}

		if( xmlDocument.SaveFile( "config.xml" ) != tinyxml2::XML_SUCCESS )
		{
			trace::write( "Failed to write config!" );
		}

		m_revision++;
	}

	void ControllerConfig::addConnection( const Connection& connection )
	{
		m_connections.pushBack( connection );

		save();
	}
}