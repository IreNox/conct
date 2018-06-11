#include "conct_array_type.h"

namespace conct
{
	ArrayType::ArrayType()
	{
		m_pBaseType = nullptr;
	}

	void ArrayType::create( const Type* pBaseType )
	{
		Type::create( ""_s, pBaseType->getName() + "[]", "Array< "_s + pBaseType->getCppName() + " >", TypeDescription_Array, ValueType_Array );
		m_pBaseType = pBaseType;
	}
}