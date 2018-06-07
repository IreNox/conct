#include "conct_value_builder.h"

#include "conct_number_limits.h"

namespace conct
{
	ValueBuilder::ValueBuilder( void* pBuffer, uintreg bufferSize )
	{
		CONCT_ASSERT( bufferSize >= sizeof( Value ) );

		m_builder.set( pBuffer, bufferSize );
		m_pValue = m_builder.pushStruct< Value >();
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
		const char* pString = m_builder.pushString( pValue );
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
}