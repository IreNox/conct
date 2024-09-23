#pragma once

#include "gen/system_clipboard.h"

namespace conct
{
	class ClipboardImpl : Clipboard
	{
	public:

		virtual const char* getText() const TIKI_OVERRIDE_FINAL;
		virtual void setText( const char* value ) TIKI_OVERRIDE_FINAL;

		virtual void Clear(  ) TIKI_OVERRIDE_FINAL;
	};
}
