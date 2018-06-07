#include "conct_data_builder.h"

#include "conct_memory.h"
#include "conct_string.h"

namespace conct
{
	DataBuilder::DataBuilder()
	{
		set( nullptr, 0u );
	}

	DataBuilder::DataBuilder( void* pData, uintreg size )
	{
		set( pData, size );
	}

	void DataBuilder::set( void* pData, uintreg size )
	{
		m_pBaseData	= static_cast< uint8* >( pData );
		m_pData		= m_pBaseData;
		m_pEnd		= m_pBaseData + size;
	}

	void* DataBuilder::pushData( uintreg length )
	{
		void* pData = m_pData;
		m_pData += length;

		if( isExceeded() )
		{
			return nullptr;
		}

		return pData;
	}

	void DataBuilder::pushData( const void* pData, uintreg length )
	{
		void* pTargetData = m_pData;
		m_pData += length;

		if( isExceeded() )
		{
			return;
		}

		memory::copy( pTargetData, pData, length );
	}

	const char* DataBuilder::pushString( const char* pString )
	{
		const uintreg stringLength = getStringLength( pString ) + 1u;

		char* pData = (char*)m_pData;
		m_pData += stringLength;

		if( isExceeded() )
		{
			return nullptr;
		}

		copyString( pData, stringLength, pString );

		return pData;
	}

	void DataBuilder::pushValueData( Value* pTargetValue, const Value* pSourceValue )
	{

	}

	ArrayView< uint8 > DataBuilder::toArrayView() const
	{
		if( isExceeded() )
		{
			return ArrayView< uint8 >();
		}

		return ArrayView< uint8 >( m_pBaseData, m_pData - m_pBaseData );
	}
}
