#pragma once

#include "conct_dynamic_string.h"
#include "conct_value.h"
#include "conct_vector.h"

namespace conct
{
	class ValueHigh
	{
	public:

						ValueHigh();
						ValueHigh( const Value& value );

		ValueType		getType() const;
		const Value&	getBaseValue() const;

		bool			getBoolean() const;
		sint32			getInteger() const;
		uint32			getUnsigned() const;
		const char*		getString() const;
		PercentValue	getPercentValue() const;
		Guid			getGuid() const;
		DeviceId		getDeviceId() const;
		InstanceId		getInstanceId() const;
		TypeCrc			getTypeCrc() const;

		const void*		getStructData() const;
		uintreg			getStructSize() const;
		TypeCrc			getStructType() const;

		const void*		getArrayData() const;
		uintreg			getArrayElementSize() const;
		uintreg			getArrayCount() const;
		TypeCrc			getArrayType() const;

		void			setVoid();
		void			setBoolean( bool value );
		void			setInteger( sint32 value );
		void			setUnsigned( uint32 value );
		void			setPercentValue( PercentValue value );
		void			setGuid( Guid value );
		void			setDeviceId( DeviceId value );
		void			setInstanceId( InstanceId value );
		void			setTypeCrc( TypeCrc value );

	private:

		Value			m_value;
		DynamicString	m_string;
		Vector< uint8 >	m_buffer;
	};
}