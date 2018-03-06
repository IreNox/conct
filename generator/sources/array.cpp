#include "array.h"

namespace conct
{
	Array::Array()
	{
		m_pBaseType = nullptr;
	}

	void Array::create( const Type* pBaseType )
	{
		Type::create( "", pBaseType->getName() + "[]", "Array< " + pBaseType->getCppName() + " >", TypeDescription_Array );
		m_pBaseType = pBaseType;
	}
}