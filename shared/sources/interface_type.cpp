#include "interface_type.h"

#include "type_collection.h"
#include "xml_helper.h"

#include <third_party/tinyxml2.h>

#include <iostream>

namespace conct
{
	InterfaceType::InterfaceType()
	{
		m_pBaseType = nullptr;
	}

	void InterfaceType::create( const std::string& fileName, const std::string& namespaceVar, const std::string& name )
	{
		Type::create( namespaceVar, name, name, TypeDescription_Interface, ValueType_Instance );
		m_fileName = fileName;
	}

	bool InterfaceType::load( TypeCollection& typeCollection )
	{
		tinyxml2::XMLDocument document;
		if( document.LoadFile( m_fileName.c_str() ) != tinyxml2::XML_SUCCESS )
		{
			std::cout << "Error: Failed to load XML from '" << m_fileName << "'. Message: " << document.ErrorStr() << std::endl;
			return false;
		}

		tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "interface" );
		if( pRootNode == nullptr )
		{
			std::cout << "Error: Failed to find root node in '" << m_fileName << "'." << std::endl;
			return false;
		}

		const Type* pBaseType = nullptr;
		if( loadTypeValue( &pBaseType, pRootNode, "baseType", getNamespace(), typeCollection, true ) )
		{
			if( pBaseType->getDescription() != TypeDescription_Interface )
			{
				std::cout << "Error: Base type of '" << getFullName() << "' is not an interface in '" << m_fileName << "'." << std::endl;
				return false;
			}

			m_pBaseType = static_cast<const InterfaceType*>( pBaseType );
		}

		tinyxml2::XMLElement* pPropertiesNode = pRootNode->FirstChildElement( "properties" );
		if( pPropertiesNode != nullptr )
		{
			tinyxml2::XMLElement* pPropertyNode = pPropertiesNode->FirstChildElement( "property" );
			while( pPropertyNode != nullptr )
			{
				std::string propertyName;
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
					std::cout << "Error: Property '" << propertyName << "' in '" << getFullName() << "' has no getter and no setter." << std::endl;
					return false;
				}

				m_properties.push_back( property );

				pPropertyNode = pPropertyNode->NextSiblingElement( "property" );
			}
		}

		tinyxml2::XMLElement* pFunctionsNode = pRootNode->FirstChildElement( "functions" );
		if( pFunctionsNode != nullptr )
		{
			tinyxml2::XMLElement* pFunctionNode = pPropertiesNode->FirstChildElement( "function" );
			while( pFunctionNode != nullptr )
			{
				std::string functionName;
				const Type* pReturnType = nullptr;
				if( !loadStringValue( functionName, pFunctionNode, "name" ) ||
					!loadTypeValue( &pReturnType, pFunctionNode, "return", getNamespace(), typeCollection ) )
				{
					return false;
				}

				InterfaceFunction function;
				function.name			= functionName;
				function.pReturnType	= pReturnType;

				tinyxml2::XMLElement* pParametersNode = pFunctionNode->FirstChildElement( "parameters" );
				if( pParametersNode != nullptr )
				{
					tinyxml2::XMLElement* pParameterNode = pParametersNode->FirstChildElement( "parameter" );
					while( pPropertiesNode != nullptr )
					{
						std::string parameterName;
						const Type* pParameterType = nullptr;
						if( !loadStringValue( parameterName, pFunctionNode, "name" ) ||
							!loadTypeValue( &pParameterType, pFunctionNode, "type", getNamespace(), typeCollection ) )
						{
							return false;
						}

						InterfaceFunctionParameter parameter;
						parameter.name	= parameterName;
						parameter.pType	= pParameterType;

						function.parameters.push_back( parameter );

						pParameterNode = pParameterNode->NextSiblingElement( "parameter" );
					}
				}

				m_functions.push_back( function );

				pFunctionNode = pFunctionNode->NextSiblingElement( "function" );
			}
		}

		return true;
	}

	//TypeCrc Interface::getCrc() const
	//{
	//
	//}
}
