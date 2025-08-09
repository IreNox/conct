#include "conct_value.h"

namespace conct
{
	Value::Value()
		: m_type( ValueType::Void )
	{
	}

	ValueType Value::getType() const
	{
		return m_type;
	}

	bool Value::getBoolean() const
	{
		TIKI_ASSERT( m_type == ValueType::Boolean );
		return m_data.boolean;
	}

	sint32 Value::getInteger() const
	{
		TIKI_ASSERT( m_type == ValueType::Integer );
		return m_data.integer;
	}

	uint32 Value::getUnsigned() const
	{
		TIKI_ASSERT( m_type == ValueType::Unsigned );
		return m_data.unsignedInteger;
	}

	StringView Value::getString() const
	{
		TIKI_ASSERT( m_type == ValueType::String );
		return StringView( (const char*)m_buffer.getData(), m_buffer.getLength() - 1u );
	}

	PercentValue Value::getPercentValue() const
	{
		TIKI_ASSERT( m_type == ValueType::PercentValue );
		return m_data.percent;
	}

	DeviceId Value::getDeviceId() const
	{
		TIKI_ASSERT( m_type == ValueType::DeviceId );
		return m_data.device;
	}

	InstanceId Value::getInstanceId() const
	{
		TIKI_ASSERT( m_type == ValueType::InstanceId );
		return m_data.instanceId;
	}

	TypeCrc Value::getTypeCrc() const
	{
		TIKI_ASSERT( m_type == ValueType::TypeCrc );
		return m_data.type;
	}

	const void* Value::getStructData() const
	{
		TIKI_ASSERT( m_type == ValueType::Struct );
		return m_buffer.getData();
	}

	uintsize Value::getStructSize() const
	{
		TIKI_ASSERT( m_type == ValueType::Struct );
		return m_data.structure.size;
	}

	TypeCrc Value::getStructType() const
	{
		TIKI_ASSERT( m_type == ValueType::Struct );
		return m_data.structure.type;
	}

	const void* Value::getArrayData() const
	{
		TIKI_ASSERT( m_type == ValueType::Array );
		return m_buffer.getData();
	}

	uintsize Value::getArrayElementSize() const
	{
		TIKI_ASSERT( m_type == ValueType::Array );
		return m_data.array.elementSize;
	}

	uintsize Value::getArrayLength() const
	{
		TIKI_ASSERT( m_type == ValueType::Array );
		return m_data.array.length;
	}

	TypeCrc Value::getArrayType() const
	{
		TIKI_ASSERT( m_type == ValueType::Array );
		return m_data.array.type;
	}

	void Value::setVoid()
	{
		m_type = ValueType::Void;
	}

	void Value::setBoolean( bool value )
	{
		m_type = ValueType::Boolean;
		m_data.boolean = value;
	}

	void Value::setInteger( sint32 value )
	{
		m_type = ValueType::Integer;
		m_data.integer = value;
	}

	void Value::setUnsigned( uint32 value )
	{
		m_type = ValueType::Unsigned;
		m_data.unsignedInteger = value;
	}

	void Value::setPercentValue( PercentValue value )
	{
		m_type = ValueType::PercentValue;
		m_data.percent = value;
	}

	void Value::setString( const char* value )
	{
		setString( StringView( value ) );
	}

	void Value::setString( const StringView& value )
	{
		m_type = ValueType::String;
		m_buffer.assign( value.cast< byte >() );
	}

	void Value::setDeviceId( DeviceId value )
	{
		m_type = ValueType::DeviceId;
		m_data.device = value;
	}

	void Value::setInstanceId( InstanceId value )
	{
		m_type = ValueType::InstanceId;
		m_data.instanceId = value;
	}

	void Value::setTypeCrc( TypeCrc value )
	{
		m_type = ValueType::TypeCrc;
		m_data.type = value;
	}

	void Value::setStructData( const void* data, uint32 size, TypeCrc typeCrc )
	{
		m_type = ValueType::Struct;
		m_data.structure.size = size;
		m_data.structure.type = typeCrc;

		m_buffer.assign( (const uint8*)data, size );
	}

	void Value::setArrayData( const void* data, uint32 elementSize, uint32 length, TypeCrc typeCrc )
	{
		m_type = ValueType::Array;
		m_data.array.elementSize	= elementSize;
		m_data.array.length			= length;
		m_data.array.type			= typeCrc;

		m_buffer.assign( (const uint8*)data, elementSize * length );
	}

	void Value::serialize( BinaryWriter& target ) const
	{
		target.writeByte( (byte)m_type );
		target.writeStruct( m_data );
		target.writeData( m_buffer.getLength() );
		target.writeData( m_buffer.getData(), m_buffer.getLength() );
	}

	bool Value::deserialize( BinaryReader& source )
	{
		byte type;
		if( !source.readByte( type ) )
		{
			return false;
		}

		if( type > (byte)ValueType::Count )
		{
			return false;
		}

		m_type = (ValueType)type;

		if( !source.readStruct( m_data ) )
		{
			return false;
		}

		uint32


		return true;
	}
}