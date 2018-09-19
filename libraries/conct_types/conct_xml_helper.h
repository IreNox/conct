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

	void	traceNodeError( tinyxml2::XMLElement* pNode, const char* pText );
	void	traceNodeError( tinyxml2::XMLElement* pNode, const DynamicString& text );

	bool	loadBooleanValue( bool& target, tinyxml2::XMLElement* pNode, const char* pName );
	bool	loadStringValue( DynamicString& target, tinyxml2::XMLElement* pNode, const char* pName, bool ignoreMissing = false );
	bool	loadMemSizeValue( uintreg& target, tinyxml2::XMLElement* pNode, const char* pName, bool ignoreMissing = false );
	bool	loadTypeValue( const Type** ppType, tinyxml2::XMLElement* pNode, const char* pName, const DynamicString& referenceNamespace, TypeCollection& typeCollection, bool ignoreMissing = false );
	bool	loadInterfaceValue( const InterfaceType** ppInterface, tinyxml2::XMLElement* pNode, const char* pName, const DynamicString& referenceNamespace, TypeCollection& typeCollection, bool ignoreMissing = false );

	template< class TEnum >
	bool	loadEnumValue( TEnum& target, tinyxml2::XMLElement* pNode, const char* pName, const ArrayView< Pair< TEnum, const char*> >& mapping, bool ignoreMissing = false );
}

#include "conct_xml_helper.inl"
