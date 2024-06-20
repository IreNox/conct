#include "conct_array_type.h"

namespace conct
{
	ArrayType::ArrayType()
	{
		m_pBaseType = nullptr;
	}

	void ArrayType::create( const Type* pBaseType )
	{
		Type::create( Path(), ""_s, pBaseType->getName() + "[]", "Array< "_s + pBaseType->getCppName() + " >", TypeKind_Array, ValueType_Array, false );
		m_pBaseType = pBaseType;
	}
}