#pragma once

#include "conct_value_builder.h"
#include "conct_data_builder.h"

namespace conct
{
	class BasicValueBuilder : public ValueBuilder
	{
	public:

					BasicValueBuilder( void* pBuffer, uintreg bufferSize );

	private:

		DataBuilder	m_builder;
	};

	template< uintreg TBufferSize >
	class BufferedValueBuilder : public BasicValueBuilder
	{
	public:

					BufferedValueBuilder();

	private:

		uint8		m_buffer[ TBufferSize ];
	};
}

#include "conct_buffered_value_builder.inl"
