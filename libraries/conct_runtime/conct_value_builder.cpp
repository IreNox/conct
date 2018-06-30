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

	ArrayView< uint8 > ValueBuilder::toArrayView() const
	{
		return ArrayView< uint8 >( (const uint8*)m_pValue, m_pBuilder->getSize() );
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
		m_pValue->setString( uint16( ( uintptr )pString - ( uintptr )m_pValue ) );

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

	ResultId ValueBuilder::setInstance( Instance value )
	{
		m_pValue->setInstance( value );
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
		m_pValue->setStruct( uint16( ( uintptr )pStruct - ( uintptr )m_pValue ), uint16( dataSize ), type );

		return ResultId_Success;
	}

	ResultId ValueBuilder::setArray( const void* pData, uintreg elementSize, uintreg length, TypeCrc type )
	{
		void* pArray = m_pBuilder->pushData( pData, elementSize * length );
		if( pArray == nullptr )
		{
			return ResultId_OutOfMemory;
		}

		CONCT_ASSERT( ( uintptr )pArray - ( uintptr )m_pValue < NumberLimits< uint16 >::max() );
		m_pValue->setArray( uint16( ( uintptr )pArray - ( uintptr )m_pValue ), uint8( elementSize ), uint8( length ), type );

		return ResultId_Success;
	}
}