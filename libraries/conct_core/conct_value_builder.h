#pragma once

#include "conct_data_builder.h"
#include "conct_result.h"
#include "conct_value.h"

namespace conct
{
	class ValueBuilder
	{
	public:

						ValueBuilder( void* pBuffer, uintreg bufferSize );

		const Value*	getValue() const { return m_pValue; }
		uintreg			getValueSize() const { return m_builder.getSize(); }

		ResultId		setVoid();
		ResultId		setBoolean( bool value );
		ResultId		setInteger( sint32 value );
		ResultId		setUnsigned( uint32 value );
		ResultId		setString( const char* pValue );
		ResultId		setPercentValue( PercentValue value );
		ResultId		setGuid( Guid value );
		ResultId		setDeviceId( DeviceId value );
		ResultId		setInstanceId( InstanceId value );
		ResultId		setTypeCrc( TypeCrc value );

		template< class T >
		ResultId		setStruct( const T& value );

		template< class T >
		ResultId		setArray( const ArrayView< T >& value );

	private:

		DataBuilder		m_builder;
		Value*			m_pValue;
	};
}

#include "conct_value_builder.inl"
