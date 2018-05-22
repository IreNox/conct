#include "array_type.h"

namespace conct
{
	ArrayType::ArrayType()
	{
		m_pBaseType = nullptr;
	}

	void ArrayType::create( const Type* pBaseType )
	{
		Type::create( "", pBaseType->getName() + "[]", "Array< " + pBaseType->getCppName() + " >", TypeDescription_Array, ValueType_Array );
		m_pBaseType = pBaseType;
	}
}