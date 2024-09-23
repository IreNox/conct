#pragma once

#include "conct_core.h"

#include "console_plugin.h"

namespace conct
{
	class ConsoleInstances : public ConsolePlugin
	{
	public:

		virtual void				activate( ConsoleDevice& device ) override final;
		virtual void				deactivate( ConsoleDevice& device ) override final;

		virtual void				update( ConsoleDevice& device ) override final;
		virtual void				draw( const ConsoleDevice& device ) const override final;

		virtual const char*			getName() const override final;
	};
}