#pragma once

#include "conct_core.h"

#include "console_plugin.h"

namespace conct
{
	class ConsoleInstances : public ConsolePlugin
	{
	public:

		virtual void				activate( ConsoleDevice& device ) CONCT_OVERRIDE_FINAL;
		virtual void				deactivate( ConsoleDevice& device ) CONCT_OVERRIDE_FINAL;

		virtual void				update( ConsoleDevice& device ) CONCT_OVERRIDE_FINAL;
		virtual void				draw( const ConsoleDevice& device ) const CONCT_OVERRIDE_FINAL;

		virtual const char*			getName() const CONCT_OVERRIDE_FINAL;
	};
}