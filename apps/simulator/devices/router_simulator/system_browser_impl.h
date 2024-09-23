#pragma once

#include "gen/system_browser.h"

namespace conct
{
	class BrowserImpl : Browser
	{
	public:

		virtual bool getIsOpen() const TIKI_OVERRIDE_FINAL;
		virtual void setIsOpen( bool value ) TIKI_OVERRIDE_FINAL;

		virtual void Open( const char* url ) TIKI_OVERRIDE_FINAL;
	};
}
