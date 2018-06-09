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
			m_string					= DynamicString( value.getString() );
			m_value.type				= ValueType_String;
			m_value.data.string.offset	= 0u;
		}
		else if( value.type == ValueType_Struct )
		{
			m_buffer.pushRange( static_cast< const uint8* >( value.getStructData() ), value.getStructSize() );

			m_value.type					= ValueType_Struct;
			m_value.data.structure.offset	= 0u;
			m_value.data.structure.size		= uint16( value.getStructSize() );
			m_value.data.structure.type		= value.getStructType();
		}
		else if( value.type == ValueType_Array )
		{
			m_buffer.pushRange( static_cast< const uint8* >( value.getArrayData() ), value.getArrayElementSize() * value.getArrayCount() );

			m_value.type				= ValueType_Array;
			m_value.data.array.offset	= 0u;
			m_value.data.array.size		= uint8( value.getArrayElementSize() );
			m_value.data.array.count	= uint8( value.getArrayCount() );
			m_value.data.array.type		= value.getArrayType();
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

	const Value& ValueHigh::getBaseValue() const
	{
		return m_value;
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

	const char* ValueHigh::getString() const
	{
		CONCT_ASSERT( m_value.type == ValueType_String );
		return m_string.toConstCharPointer();
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

	InstanceId ValueHigh::getInstanceId() const
	{
		return m_value.getInstanceId();
	}

	TypeCrc ValueHigh::getTypeCrc() const
	{
		return m_value.getTypeCrc();
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

	uintreg ValueHigh::getArrayCount() const
	{
		return m_value.getArrayCount();
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

	void ValueHigh::setInstanceId( InstanceId value )
	{
		m_value.setInstanceId( value );
	}

	void ValueHigh::setTypeCrc( TypeCrc value )
	{
		m_value.setTypeCrc( value );
	}
}