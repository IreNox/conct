#pragma  once

#include "conct_core.h"

#include <tiki/tiki_dynamic_string.h>
#include <tiki/tiki_pair.h>
#include <tinyxml2/tinyxml2.h>

namespace tiki
{
	class DynamicString;
	class Path;
}

namespace conct
{
	class InterfaceType;
	class Type;
	class TypeCollection;

	bool	loadDocument( tinyxml2::XMLDocument& document, const Path& path );

	void	traceNodeError( const tinyxml2::XMLElement* pNode, const char* pText );
	void	traceNodeError( const tinyxml2::XMLElement* pNode, const DynamicString& text );

	bool	loadBooleanValue( bool& target, const tinyxml2::XMLElement* pNode, const char* pName );
	bool	loadStringValue( DynamicString& target, const tinyxml2::XMLElement* pNode, const char* pName, bool ignoreMissing = false );
	bool	loadMemSizeValue( uintreg& target, const tinyxml2::XMLElement* pNode, const char* pName, bool ignoreMissing = false );
	bool	loadIntegerValue( sint64& target, const tinyxml2::XMLElement* pNode, const char* pName, bool ignoreMissing = false );
	bool	loadTypeValue( const Type** ppType, const tinyxml2::XMLElement* pNode, const char* pName, const DynamicString& referenceNamespace, TypeCollection& typeCollection, bool ignoreMissing = false );
	bool	loadInterfaceValue( const InterfaceType** ppInterface, const tinyxml2::XMLElement* pNode, const char* pName, const DynamicString& referenceNamespace, TypeCollection& typeCollection, bool ignoreMissing = false );

	template< class TEnum >
	bool	loadEnumValue( TEnum& target, const tinyxml2::XMLElement* pNode, const char* pName, const ArrayView< const Pair< TEnum, const char* > >& mapping, bool ignoreMissing = false );
}

#include "conct_xml_helper.inl"
