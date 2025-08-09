#pragma once

#include "conct_value_type.h"

#include <tiki/tiki_dynamic_string.h>

namespace conct
{
	class BinaryReader;
	class BinaryWriter;

	struct ValueStructData
	{
		uint32				size;
		TypeCrc				type;
	};

	struct ValueArrayData
	{
		uint32				length;
		uint16				elementSize;
		TypeCrc				type;
	};

	union ValueData
	{
		bool				boolean;
		sint32				integer;
		uint32				unsignedInteger;
		PercentValue		percent;
		DeviceId			device;
		InstanceId			instanceId;
		TypeCrc				type;
		ValueStructData		structure;
		ValueArrayData		array;
	};

	struct ValueSerializeData
	{
		ValueType			type;
		ValueData			data;
		uint32				bufferOffset;
	};

	class Value
	{
	public:

							Value();

		ValueType			getType() const;

		bool				getBoolean() const;
		sint32				getInteger() const;
		uint32				getUnsigned() const;
		StringView			getString() const;
		PercentValue		getPercentValue() const;
		DeviceId			getDeviceId() const;
		InstanceId			getInstanceId() const;
		TypeCrc				getTypeCrc() const;

		template< class T >
		const T&			getStruct() const;
		const void*			getStructData() const;
		uintsize			getStructSize() const;
		TypeCrc				getStructType() const;

		template< class T >
		ConstArrayView< T >	getArray() const;
		const void*			getArrayData() const;
		uintsize			getArrayElementSize() const;
		uintsize			getArrayLength() const;
		TypeCrc				getArrayType() const;

		void				setVoid();
		void				setBoolean( bool value );
		void				setInteger( sint32 value );
		void				setUnsigned( uint32 value );
		void				setString( const char* value );
		void				setString( const StringView& value );
		void				setPercentValue( PercentValue value );
		void				setDeviceId( DeviceId value );
		void				setInstanceId( InstanceId value );
		void				setTypeCrc( TypeCrc value );
		template< class T >
		void				setStruct( const T& value );
		void				setStructData( const void* data, uint32 size, TypeCrc type );
		template< class T >
		void				setArray( const ConstArrayView< T >& value );
		void				setArrayData( const void* data, uint32 elementSize, uint32 length, TypeCrc type );

		void				serialize( BinaryWriter& target ) const;
		bool				deserialize( BinaryReader& source );

	private:

		//using ByteArray = DynamicArray< byte >;

		ValueType		m_type;
		bool			m_ownsBuffer;
		ValueData		m_data;
		byte*			m_buffer;
		uintsize		m_bufferLength;
		uintsize		m_bufferCapacity;

		//ByteArray		m_buffer;
	};
}

#include "conct_value.inl"
