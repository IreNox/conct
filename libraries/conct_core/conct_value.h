#pragma once

#include "conct_core.h"

namespace conct
{
	enum ValueType : uint8
	{
		ValueType_Void,
		ValueType_Boolean,
		ValueType_Integer,
		ValueType_Unsigned,
		ValueType_String,
		ValueType_PercentValue,
		ValueType_Guid,
		ValueType_Device,
		ValueType_Instance,
		ValueType_Type,
		ValueType_Struct,
		ValueType_Array,

		ValueType_Count
	};
	const char*	getValueTypeName( ValueType value );

	struct StringValueData
	{
		uint16	offset;
	};

	struct StructValueData
	{
		uint16	offset;
	};

	struct ArrayValueData
	{
		uint16	offset;
		uint16	size;
	};

	union ValueData
	{
		bool				boolean;
		sint32				integer;
		uint32				unsignedInteger;
		StringValueData		string;
		PercentValue		percent;
		Guid				guid;
		DeviceId			device;
		InstanceId			instance;
		TypeCrc				type;
		StructValueData		structure;
		ArrayValueData		array;
	};

	struct Value
	{
		ValueType		type;
		ValueData		data;

		bool			getBoolean() const;
		sint32			getInteger() const;
		uint32			getUnsigned() const;
		PercentValue	getPercentValue() const;
		Guid			setGuid() const;
		DeviceId		getDeviceId() const;
		InstanceId		getInstanceId() const;
		TypeCrc			getTypeCrc() const;

		void			setVoid();
		void			setBoolean( bool value );
		void			setInteger( sint32 value );
		void			setUnsigned( uint32 value );
		void			setPercentValue( PercentValue value );
		void			setGuid( Guid value );
		void			setDeviceId( DeviceId value );
		void			setInstanceId( InstanceId value );
		void			setTypeCrc( TypeCrc value );
	};
}