#pragma once

#include "conct_core.h"

namespace conct
{
	class Clipboard
	{
	public:

		static const TypeCrc s_typeCrc = 0x676e;

		virtual const char* getText() const = 0;
		virtual void setText( const char* value ) = 0;

		virtual void Clear(  ) = 0;
	};
}
