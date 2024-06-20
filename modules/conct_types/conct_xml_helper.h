#pragma  once

#include "conct_array_view.h"
#include "conct_core.h"
#include "conct_dynamic_string.h"
#include "conct_pair.h"

#include <tinyxml2/tinyxml2.h>

namespace conct
{
	class DynamicString;
	class InterfaceType;
	class Path;
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
	bool	loadEnumValue( TEnum& target, const tinyxml2::XMLElement* pNode, const char* pName, const ArrayView< Pair< TEnum, const char*> >& mapping, bool ignoreMissing = false );
}

#include "conct_xml_helper.inl"
