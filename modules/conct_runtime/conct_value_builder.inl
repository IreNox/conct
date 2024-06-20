#pragma once

#include "conct_number_limits.h"
#include "conct_value_triats.h"

namespace conct
{
	template< class T >
	ResultId ValueBuilder::setStruct( const T& value )
	{
		return setStruct( &value, sizeof( value ), ValueTypeTraits< T >::getTypeCrc() );
	}

	template< class T >
	ResultId ValueBuilder::setArray( const ArrayView< T >& value )
	{
		return setArray( value.getData(), sizeof( T ), value.getLength(), ValueTypeTraits< T >::getTypeCrc() );
	}

	template< class T >
	ResultId ValueBuilder::setValue( const T* pValue )
	{
		switch( pValue->getType() )
		{
		case ValueType_Void:
			return setVoid();

		case ValueType_Boolean:
			return setBoolean( pValue->getBoolean() );

		case ValueType_Integer:
			return setInteger( pValue->getInteger() );

		case ValueType_Unsigned:
			return setUnsigned( pValue->getUnsigned() );

		case ValueType_String:
			return setString( pValue->getString() );

		case ValueType_PercentValue:
			return setPercentValue( pValue->getPercentValue() );

		case ValueType_DeviceId:
			return setDeviceId( pValue->getDeviceId() );

		case ValueType_InstanceId:
			return setInstanceId( pValue->getInstanceId() );

		case ValueType_TypeCrc:
			return setTypeCrc( pValue->getTypeCrc() );

		case ValueType_Struct:
			return setStruct( pValue->getStructData(), pValue->getStructSize(), pValue->getStructType() );

		case ValueType_Array:
			return setArray( pValue->getArrayData(), pValue->getArrayElementSize(), pValue->getArrayLength(), pValue->getArrayType() );

		case ValueType_Count:
			break;
		}

		return ResultId_Unknown;
	}
}