#pragma once

#include "conct_value_triats.h"

#include <tiki/tiki_number_limits.h>

namespace conct
{
	template< class T >
	ResultId ValueBuilder::setStruct( const T& value )
	{
		return setStruct( &value, sizeof( value ), ValueTypeTraits< T >::getTypeCrc() );
	}

	template< class T >
	ResultId ValueBuilder::setArray( const ArrayView< const T >& value )
	{
		return setArray( value.getData(), sizeof( T ), value.getLength(), ValueTypeTraits< T >::getTypeCrc() );
	}

	template< class T >
	ResultId ValueBuilder::setValue( const T* pValue )
	{
		switch( pValue->getType() )
		{
		case ValueType::Void:
			return setVoid();

		case ValueType::Boolean:
			return setBoolean( pValue->getBoolean() );

		case ValueType::Integer:
			return setInteger( pValue->getInteger() );

		case ValueType::Unsigned:
			return setUnsigned( pValue->getUnsigned() );

		case ValueType::String:
			return setString( pValue->getString() );

		case ValueType::PercentValue:
			return setPercentValue( pValue->getPercentValue() );

		case ValueType::DeviceId:
			return setDeviceId( pValue->getDeviceId() );

		case ValueType::InstanceId:
			return setInstanceId( pValue->getInstanceId() );

		case ValueType::TypeCrc:
			return setTypeCrc( pValue->getTypeCrc() );

		case ValueType::Struct:
			return setStruct( pValue->getStructData(), pValue->getStructSize(), pValue->getStructType() );

		case ValueType::Array:
			return setArray( pValue->getArrayData(), pValue->getArrayElementSize(), pValue->getArrayLength(), pValue->getArrayType() );

		case ValueType::Count:
			break;
		}

		return ResultId_Unknown;
	}
}