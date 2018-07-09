#include "conct_value_high.h"

#include "conct_functions.h"

namespace conct
{
	ValueHigh::ValueHigh()
	{
		m_value.setVoid();
	}

	ValueHigh::ValueHigh( const Value& value )
	{
		if( value.type == ValueType_String )
		{
			m_string = DynamicString( value.getString() );
			m_value.setString( 0u );
		}
		else if( value.type == ValueType_Struct )
		{
			m_buffer.pushRange( static_cast< const uint8* >( value.getStructData() ), value.getStructSize() );
			m_value.setStruct( 0u, value.getStructSize(), value.getStructType() );
		}
		else if( value.type == ValueType_Array )
		{
			m_buffer.pushRange( static_cast< const uint8* >( value.getArrayData() ), value.getArrayElementSize() * value.getArrayLength() );
			m_value.setArray( 0u, value.getArrayElementSize(), value.getArrayLength(), value.getArrayType() );
		}
		else
		{
			m_value = value;
		}
	}

	ValueType ValueHigh::getType() const
	{
		return m_value.type;
	}

	bool ValueHigh::getBoolean() const
	{
		return m_value.getBoolean();
	}

	sint32 ValueHigh::getInteger() const
	{
		return m_value.getInteger();
	}

	uint32 ValueHigh::getUnsigned() const
	{
		return m_value.getUnsigned();
	}

	PercentValue ValueHigh::getPercentValue() const
	{
		return m_value.getPercentValue();
	}

	Guid ValueHigh::getGuid() const
	{
		return m_value.getGuid();
	}

	DeviceId ValueHigh::getDeviceId() const
	{
		return m_value.getDeviceId();
	}

	Instance ValueHigh::getInstance() const
	{
		return m_value.getInstance();
	}

	TypeCrc ValueHigh::getTypeCrc() const
	{
		return m_value.getTypeCrc();
	}

	const char* ValueHigh::getString() const
	{
		CONCT_ASSERT( m_value.type == ValueType_String );
		return m_string.toConstCharPointer();
	}

	const void* ValueHigh::getStructData() const
	{
		CONCT_ASSERT( m_value.type == ValueType_Struct );
		return m_buffer.getData();
	}

	uintreg ValueHigh::getStructSize() const
	{
		return m_value.getStructSize();
	}

	TypeCrc ValueHigh::getStructType() const
	{
		return m_value.getStructType();
	}

	const void* ValueHigh::getArrayData() const
	{
		CONCT_ASSERT( m_value.type == ValueType_Array );
		return m_buffer.getData();
	}

	uintreg ValueHigh::getArrayElementSize() const
	{
		return m_value.getArrayElementSize();
	}

	uintreg ValueHigh::getArrayLength() const
	{
		return m_value.getArrayLength();
	}

	TypeCrc ValueHigh::getArrayType() const
	{
		return m_value.getArrayType();
	}

	void ValueHigh::setVoid()
	{
		m_value.setVoid();
	}

	void ValueHigh::setBoolean( bool value )
	{
		m_value.setBoolean( value );
	}

	void ValueHigh::setInteger( sint32 value )
	{
		m_value.setInteger( value );
	}

	void ValueHigh::setUnsigned( uint32 value )
	{
		m_value.setUnsigned( value );
	}

	void ValueHigh::setString( const char* pValue )
	{
		m_string = DynamicString( pValue );
		m_value.setString( 0u );
	}

	void ValueHigh::setPercentValue( PercentValue value )
	{
		m_value.setPercentValue( value );
	}

	void ValueHigh::setGuid( Guid value )
	{
		m_value.setGuid( value );
	}

	void ValueHigh::setDeviceId( DeviceId value )
	{
		m_value.setDeviceId( value );
	}

	void ValueHigh::setInstance( Instance value )
	{
		m_value.setInstance( value );
	}

	void ValueHigh::setTypeCrc( TypeCrc value )
	{
		m_value.setTypeCrc( value );
	}

	void ValueHigh::setStruct( const void* pData, uintreg size, TypeCrc type )
	{
		m_buffer.clear();
		m_buffer.pushRange( (const uint8*)pData, size );
		m_value.setStruct( 0u, uint16( size ), type );
	}

	void ValueHigh::setArray( const void* pData, uintreg elementSize, uintreg length, TypeCrc type )
	{
		m_buffer.clear();
		m_buffer.pushRange( (const uint8*)pData, elementSize * length );
		m_value.setArray( 0u, uint8( elementSize ), uint8( length ), type );
	}
}