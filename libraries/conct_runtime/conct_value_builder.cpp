#include "conct_value_builder.h"

#include "conct_number_limits.h"
#include "conct_data_builder.h"

namespace conct
{
	ValueBuilder::ValueBuilder( DataBuilder* pBuilder, Value* pValue )
	{
		m_pBuilder = pBuilder;
		m_pValue = pValue;
	}

	uintreg ValueBuilder::getValueSize() const
	{
		return m_pBuilder->getSize();
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
		m_pValue->type					= ValueType_String;
		m_pValue->data.string.offset	= uint16( ( uintptr )pString - ( uintptr )m_pValue );

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

	ResultId ValueBuilder::setStruct( const void* pData, uintreg dataSize, TypeCrc type )
	{
		void* pStruct = m_pBuilder->pushData( pData, dataSize );
		if( pStruct == nullptr )
		{
			return ResultId_OutOfMemory;
		}

		CONCT_ASSERT( ( uintptr )pStruct - ( uintptr )m_pValue < NumberLimits< uint16 >::max() );
		m_pValue->type					= ValueType_Struct;
		m_pValue->data.structure.offset	= uint16( ( uintptr )pStruct - ( uintptr )m_pValue );
		m_pValue->data.structure.size	= uint16( dataSize );
		m_pValue->data.structure.type	= type;

		return ResultId_Success;
	}

	ResultId ValueBuilder::setArray( const void* pData, uintreg elementSize, uintreg count, TypeCrc type )
	{
		void* pArray = m_pBuilder->pushData( pData, elementSize * count );
		if( pArray == nullptr )
		{
			return ResultId_OutOfMemory;
		}

		CONCT_ASSERT( ( uintptr )pArray - ( uintptr )m_pValue < NumberLimits< uint16 >::max() );
		m_pValue->type				= ValueType_Array;
		m_pValue->data.array.offset	= uint16( ( uintptr )pArray - ( uintptr )m_pValue );
		m_pValue->data.array.size	= uint8( elementSize );
		m_pValue->data.array.count	= uint8( count );
		m_pValue->data.array.type	= type;

		return ResultId_Success;
	}
}