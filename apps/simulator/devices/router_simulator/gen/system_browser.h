#pragma once

#include "conct_core.h"

namespace conct
{
	class Browser
	{
	public:

		static const TypeCrc s_typeCrc = 0x4d96;

		virtual bool getIsOpen() const = 0;
		virtual void setIsOpen( bool value ) = 0;

		virtual void Open( const char* url ) = 0;
	};
}
