#pragma  once

#include "base.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace conct
{
	class Type;
	class TypeCollection;

	bool	loadBooleanValue( bool& target, tinyxml2::XMLElement* pNode, const char* pName );
	bool	loadStringValue( std::string& target, tinyxml2::XMLElement* pNode, const char* pName );
	bool	loadMemSizeValue( size_t& target, tinyxml2::XMLElement* pNode, const char* pName );
	bool	loadTypeValue( const Type** ppType, tinyxml2::XMLElement* pNode, const char* pName, const std::string& referenceNamespace, TypeCollection& typeCollection, bool ignoreMissing = false );
	bool	loadInterfaceValue( const Interface** ppInterface, tinyxml2::XMLElement* pNode, const char* pName, const std::string& referenceNamespace, TypeCollection& typeCollection, bool ignoreMissing = false );
}