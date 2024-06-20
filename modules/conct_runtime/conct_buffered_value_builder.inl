#pragma once

namespace conct
{
	template< uintreg TBufferSize >
	BufferedValueBuilder< TBufferSize >::BufferedValueBuilder()
		: BasicValueBuilder( m_buffer, sizeof( m_buffer ) )
	{
	}
}