#pragma  once

namespace tinyxml2
{
	class XMLElement;
}

namespace conct
{
	class DynamicString;
	class InterfaceType;
	class Type;
	class TypeCollection;

	bool	loadBooleanValue( bool& target, tinyxml2::XMLElement* pNode, const char* pName );
	bool	loadStringValue( DynamicString& target, tinyxml2::XMLElement* pNode, const char* pName );
	bool	loadMemSizeValue( size_t& target, tinyxml2::XMLElement* pNode, const char* pName );
	bool	loadTypeValue( const Type** ppType, tinyxml2::XMLElement* pNode, const char* pName, const DynamicString& referenceNamespace, TypeCollection& typeCollection, bool ignoreMissing = false );
	bool	loadInterfaceValue( const InterfaceType** ppInterface, tinyxml2::XMLElement* pNode, const char* pName, const DynamicString& referenceNamespace, TypeCollection& typeCollection, bool ignoreMissing = false );
}