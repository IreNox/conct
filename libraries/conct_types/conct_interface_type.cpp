#include "conct_interface_type.h"

#include "conct_trace.h"
#include "conct_type_collection.h"
#include "conct_xml_helper.h"

#include <tinyxml2/tinyxml2.h>

namespace conct
{
	InterfaceType::InterfaceType()
	{
		m_pBaseType = nullptr;
		m_internal = false;
		m_singleton = false;
	}

	void InterfaceType::create( const Path& fileName, const DynamicString& namespaceVar, const DynamicString& name )
	{
		Type::create( fileName, namespaceVar, name, name, TypeKind_Interface, ValueType_InstanceId, false );
	}

	bool InterfaceType::load( TypeCollection& typeCollection )
	{
		tinyxml2::XMLDocument document;
		if( !loadDocument( document, m_fileName ) )
		{
			return false;
		}

		const tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "interface" );
		if( pRootNode == nullptr )
		{
			trace::write( "Error: Failed to find root node in '"_s + m_fileName.getGenericPath() + "'." + "\n" );
			return false;
		}

		if( !loadInternal( pRootNode ) )
		{
			return false;
		}

		const Type* pBaseType = nullptr;
		if( loadTypeValue( &pBaseType, pRootNode, "base", getNamespace(), typeCollection, true ) )
		{
			if( pBaseType->getKind() != TypeKind_Interface )
			{
				trace::write( "Error: Base type of '"_s + getFullName() + "' is not an interface in '" + m_fileName.getGenericPath() + "'." + "\n" );
				return false;
			}

			m_pBaseType = static_cast<const InterfaceType*>( pBaseType );
			pushDependingType( pBaseType );
		}

		loadBooleanValue( m_singleton, pRootNode, "singleton" );

		const tinyxml2::XMLElement* pPropertiesNode = pRootNode->FirstChildElement( "properties" );
		if( pPropertiesNode != nullptr )
		{
			for( const tinyxml2::XMLElement* pPropertyNode = pPropertiesNode->FirstChildElement( "property" ); pPropertyNode != nullptr; pPropertyNode = pPropertyNode->NextSiblingElement( "property" ) )
			{
				DynamicString propertyName;
				const Type* pType = nullptr;
				if( !loadStringValue( propertyName, pPropertyNode, "name" ) ||
					!loadTypeValue( &pType, pPropertyNode, "type", getNamespace(), typeCollection ) )
				{
					return false;
				}

				InterfaceProperty property;
				property.name		= propertyName;
				property.pType		= pType;
				property.hasGetter	= (pPropertyNode->FirstChildElement( "get" ) != nullptr);
				property.hasSetter	= (pPropertyNode->FirstChildElement( "set" ) != nullptr);

				if( !property.hasGetter && !property.hasSetter )
				{
					trace::write( "Error: Property '"_s + propertyName + "' in '" + getFullName() + "' has no getter and no setter." + "\n" );
					return false;
				}

				m_properties.pushBack( property );
				pushDependingType( pType );
			}
		}

		const tinyxml2::XMLElement* pFunctionsNode = pRootNode->FirstChildElement( "functions" );
		if( pFunctionsNode != nullptr )
		{
			for( const tinyxml2::XMLElement* pFunctionNode = pFunctionsNode->FirstChildElement( "function" ); pFunctionNode != nullptr; pFunctionNode = pFunctionNode->NextSiblingElement( "function" ) )
			{
				DynamicString functionName;
				const Type* pReturnType = nullptr;
				if( !loadStringValue( functionName, pFunctionNode, "name" ) ||
					!loadTypeValue( &pReturnType, pFunctionNode, "return", getNamespace(), typeCollection ) )
				{
					return false;
				}

				InterfaceFunction function;
				function.name			= functionName;
				function.pReturnType	= pReturnType;

				const tinyxml2::XMLElement* pParametersNode = pFunctionNode->FirstChildElement( "parameters" );
				if( pParametersNode != nullptr )
				{
					for( const tinyxml2::XMLElement* pParameterNode = pParametersNode->FirstChildElement( "parameter" ); pParameterNode != nullptr; pParameterNode = pParameterNode->NextSiblingElement( "parameter" ) )
					{
						DynamicString parameterName;
						const Type* pParameterType = nullptr;
						if( !loadStringValue( parameterName, pParameterNode, "name" ) ||
							!loadTypeValue( &pParameterType, pParameterNode, "type", getNamespace(), typeCollection ) )
						{
							return false;
						}

						InterfaceFunctionParameter parameter;
						parameter.name	= parameterName;
						parameter.pType	= pParameterType;

						function.parameters.pushBack( parameter );
						pushDependingType( pParameterType );
					}
				}

				m_functions.pushBack( function );
				pushDependingType( pReturnType );
			}
		}

		const tinyxml2::XMLElement* pEventsNode = pRootNode->FirstChildElement( "events" );
		if( pEventsNode != nullptr )
		{
			for( const tinyxml2::XMLElement* pEventNode = pEventsNode->FirstChildElement( "event" ); pEventNode != nullptr; pEventNode = pEventNode->NextSiblingElement( "event" ) )
			{
				DynamicString eventName;
				const Type* pEventType = nullptr;
				if( !loadStringValue( eventName, pEventNode, "name" ) ||
					!loadTypeValue( &pEventType, pEventNode, "type", getNamespace(), typeCollection ) )
				{
					return false;
				}

				InterfaceEvent event;
				event.name	= eventName;
				event.pType	= pEventType;

				m_events.pushBack( event );
				pushDependingType( pEventType );
			}
		}

		return true;
	}
}
