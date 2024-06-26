#include "conct_buffered_value_builder.h"

namespace conct
{
	BasicValueBuilder::BasicValueBuilder( void* pBuffer, uintreg bufferSize )
		: ValueBuilder( &m_builder, static_cast< Value* >( pBuffer ) )
	{
		TIKI_ASSERT( bufferSize >= sizeof( Value ) );

		m_builder.set( pBuffer, bufferSize );

		Value* pValue = m_builder.pushStruct< Value >();
		TIKI_ASSERT( pValue == pBuffer );
		TIKI_USE( pValue );
	}
}