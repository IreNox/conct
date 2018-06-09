#pragma once

#include "conct_value_builder.h"
#include "conct_data_builder.h"

namespace conct
{
	class BufferedValueBuilder : public ValueBuilder
	{
	public:

					BufferedValueBuilder( void* pBuffer, uintreg bufferSize );

	private:

		DataBuilder	m_builder;
	};
}
