#include "array.h"

namespace conct
{
	Array::Array()
	{
		m_pBaseType = nullptr;
	}

	void Array::create( const std::string& namespaceVar, const std::string& name, const Type* pBaseType )
	{
		Type::create( namespaceVar, name, TypeDescription_Array );
		m_pBaseType = pBaseType;
	}
}