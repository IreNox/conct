#include "conct_buffered_value_builder.h"

namespace conct
{
	BasicValueBuilder::BasicValueBuilder( void* pBuffer, uintreg bufferSize )
		: ValueBuilder( &m_builder, static_cast< Value* >( pBuffer ) )
	{
		CONCT_ASSERT( bufferSize >= sizeof( Value ) );

		m_builder.set( pBuffer, bufferSize );

		Value* pValue = m_builder.pushStruct< Value >();
		CONCT_ASSERT( pValue == pBuffer );
		CONCT_USE( pValue );
	}
}