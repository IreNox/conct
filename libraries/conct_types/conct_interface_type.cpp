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
		Type::create( namespaceVar, name, name, TypeDescription_Interface, ValueType_Instance );
		m_fileName = fileName;
	}

	bool InterfaceType::load( TypeCollection& typeCollection )
	{
		tinyxml2::XMLDocument document;
		if( document.LoadFile( m_fileName.getNativePath().toConstCharPointer() ) != tinyxml2::XML_SUCCESS )
		{
			trace::write( "Error: Failed to load XML from '"_s + m_fileName.getGenericPath() + "'. Message: " + document.ErrorStr() + "\n" );
			return false;
		}

		tinyxml2::XMLElement* pRootNode = document.FirstChildElement( "interface" );
		if( pRootNode == nullptr )
		{
			trace::write( "Error: Failed to find root node in '"_s + m_fileName.getGenericPath() + "'." + "\n" );
			return false;
		}

		const Type* pBaseType = nullptr;
		if( loadTypeValue( &pBaseType, pRootNode, "baseType", getNamespace(), typeCollection, true ) )
		{
			if( pBaseType->getDescription() != TypeDescription_Interface )
			{
				trace::write( "Error: Base type of '"_s + getFullName() + "' is not an interface in '" + m_fileName.getGenericPath() + "'." + "\n" );
				return false;
			}

			m_pBaseType = static_cast<const InterfaceType*>( pBaseType );
			pushDependingType( pBaseType );
		}

		tinyxml2::XMLElement* pInternalNode = pRootNode->FirstChildElement( "internal" );
		if( pInternalNode != nullptr )
		{
			if( !loadStringValue( m_headerFilename, pInternalNode, "include" ) ||
				!loadStringValue( m_cppName, pInternalNode, "class" ) )
			{
				trace::write( "Error: Internal type data not complete type of '"_s + getFullName() + "' in '" + m_fileName.getGenericPath() + "'." + "\n" );
				return false;
			}

			m_internal = true;
		}

		loadBooleanValue( m_singleton, pRootNode, "singleton" );

		tinyxml2::XMLElement* pPropertiesNode = pRootNode->FirstChildElement( "properties" );
		if( pPropertiesNode != nullptr )
		{
			tinyxml2::XMLElement* pPropertyNode = pPropertiesNode->FirstChildElement( "property" );
			while( pPropertyNode != nullptr )
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

				pPropertyNode = pPropertyNode->NextSiblingElement( "property" );
			}
		}

		tinyxml2::XMLElement* pFunctionsNode = pRootNode->FirstChildElement( "functions" );
		if( pFunctionsNode != nullptr )
		{
			tinyxml2::XMLElement* pFunctionNode = pFunctionsNode->FirstChildElement( "function" );
			while( pFunctionNode != nullptr )
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

				tinyxml2::XMLElement* pParametersNode = pFunctionNode->FirstChildElement( "parameters" );
				if( pParametersNode != nullptr )
				{
					tinyxml2::XMLElement* pParameterNode = pParametersNode->FirstChildElement( "parameter" );
					while( pParameterNode != nullptr )
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

						pParameterNode = pParameterNode->NextSiblingElement( "parameter" );
					}
				}

				m_functions.pushBack( function );
				pushDependingType( pReturnType );

				pFunctionNode = pFunctionNode->NextSiblingElement( "function" );
			}
		}

		return true;
	}
}
