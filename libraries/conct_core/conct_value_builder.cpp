#include "conct_value_builder.h"

#include "conct_number_limits.h"

namespace conct
{
	ValueBuilder::ValueBuilder( void* pBuffer, uintreg bufferSize )
	{
		CONCT_ASSERT( bufferSize >= sizeof( Value ) );

		m_builder.set( pBuffer, bufferSize );
		m_pBuilder = &m_builder;
		m_pValue = m_builder.pushStruct< Value >();
	}

	ValueBuilder::ValueBuilder( DataBuilder* pBuilder, Value* pValue )
	{
		m_pBuilder = pBuilder;
		m_pValue = pValue;
	}

	ResultId ValueBuilder::setVoid()
	{
		m_pValue->setVoid();
		return ResultId_Success;
	}

	ResultId ValueBuilder::setBoolean( bool value )
	{
		m_pValue->setBoolean( value );
		return ResultId_Success;
	}

	ResultId ValueBuilder::setInteger( sint32 value )
	{
		m_pValue->setInteger( value );
		return ResultId_Success;
	}

	ResultId ValueBuilder::setUnsigned( uint32 value )
	{
		m_pValue->setUnsigned( value );
		return ResultId_Success;
	}

	ResultId ValueBuilder::setString( const char* pValue )
	{
		const char* pString = m_pBuilder->pushString( pValue );
		if( pString == nullptr )
		{
			return ResultId_OutOfMemory;
		}

		CONCT_ASSERT( ( uintptr )pString - ( uintptr )m_pValue < NumberLimits< uint16 >::max() );
		m_pValue->type = ValueType_String;
		m_pValue->data.string.offset = uint16( ( uintptr )pString - ( uintptr )m_pValue );

		return ResultId_Success;
	}

	ResultId ValueBuilder::setPercentValue( PercentValue value )
	{
		m_pValue->setPercentValue( value );
		return ResultId_Success;
	}

	ResultId ValueBuilder::setGuid( Guid value )
	{
		m_pValue->setGuid( value );
		return ResultId_Success;
	}

	ResultId ValueBuilder::setDeviceId( DeviceId value )
	{
		m_pValue->setDeviceId( value );
		return ResultId_Success;
	}

	ResultId ValueBuilder::setInstanceId( InstanceId value )
	{
		m_pValue->setInstanceId( value );
		return ResultId_Success;
	}

	ResultId ValueBuilder::setTypeCrc( TypeCrc value )
	{
		m_pValue->setTypeCrc( value );
		return ResultId_Success;
	}

	ResultId ValueBuilder::setStruct( const void* pData, uintreg dataSize )
	{
		void* pStruct = m_pBuilder->pushData( pData, dataSize );
		if( pStruct == nullptr )
		{
			return ResultId_OutOfMemory;
		}

		CONCT_ASSERT( ( uintptr )pStruct - ( uintptr )m_pValue < NumberLimits< uint16 >::max() );
		m_pValue->type					= ValueType_Struct;
		m_pValue->data.structure.offset	= uint16( ( uintptr )pStruct - ( uintptr )m_pValue );
		m_pValue->data.structure.size	= dataSize;

		return ResultId_Success;
	}

	ResultId ValueBuilder::setArray( const void* pData, uintreg elementSize, uintreg count )
	{
		void* pArray = m_pBuilder->pushData( pData, elementSize * count );
		if( pArray == nullptr )
		{
			return ResultId_OutOfMemory;
		}

		CONCT_ASSERT( ( uintptr )pArray - ( uintptr )m_pValue < NumberLimits< uint16 >::max() );
		m_pValue->type				= ValueType_Array;
		m_pValue->data.array.offset	= uint16( ( uintptr )pArray - ( uintptr )m_pValue );
		m_pValue->data.array.size	= elementSize;
		m_pValue->data.array.count	= count;

		return ResultId_Success;
	}

	ResultId ValueBuilder::setValue( const Value* pValue )
	{
		switch( pValue->type )
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

		case ValueType_Guid:
			return setGuid( pValue->getGuid() );

		case ValueType_DeviceId:
			return setDeviceId( pValue->getDeviceId() );

		case ValueType_InstanceId:
			return setInstanceId( pValue->getInstanceId() );

		case ValueType_TypeCrc:
			return setTypeCrc( pValue->getTypeCrc() );

		case ValueType_Struct:
			return setStruct( pValue->getStructData(), pValue->getStructSize() );

		case ValueType_Array:
			return setArray( pValue->getArrayData(), pValue->getArrayElementSize(), pValue->getArrayCount() );

		case ValueType_Count:
			break;
		}

		return ResultId_Unknown;
	}
}