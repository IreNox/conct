#pragma once

#include "conct_core.h"
#include "conct_runtime.h"
#include "conct_structs.h"

#include "console_plugin.h"

#include <string>
#include <vector>
#include <deque>

namespace conct
{
	class ConsoleController : public ConsolePlugin
	{
	public:

									ConsoleController();

		virtual void				activate( ConsoleDevice& device ) CONCT_OVERRIDE_FINAL;
		virtual void				deactivate( ConsoleDevice& device ) CONCT_OVERRIDE_FINAL;

		virtual void				update( ConsoleDevice& device ) CONCT_OVERRIDE_FINAL;
		virtual void				draw( const ConsoleDevice& device ) const CONCT_OVERRIDE_FINAL;

		virtual const char*			getName() const CONCT_OVERRIDE_FINAL;

	private:

		typedef void ( ConsoleController::*CommandFunc )( ConsoleDevice& device, const std::vector< std::string >& arguments );

		struct Command
		{
			const char*				pCommand;
			CommandFunc				pFunction;
		};

		struct ControllerInstance
		{
			std::string				name;
			TypeCrc					type;
			RemoteInstance			instance;
		};

		typedef std::vector< ControllerInstance > ControllerInstanceVector;

		static const Command					s_aCommands[];

		ControllerInstanceVector				m_instances;
		uint16									m_instancesWidth;
		bool									m_drawInstances;

		std::deque< std::string >				m_log;
		bool									m_drawLog;

		std::vector< std::string >				m_commandHistory;
		std::string								m_commandText;
		bool									m_drawCommand;

		void									drawInstances( const uint16x2 size ) const;
		void									drawLog( const uint16x2 size ) const;
		void									drawCommand( const uint16x2 size ) const;

		void									updateInstancesWidth();

		void									pushLog( const char* pText );

		void									autoComplete( ConsoleDevice& device );

		void									executeCommand( ConsoleDevice& device );
		void									executeHelpCommand( ConsoleDevice& device, const std::vector< std::string >& arguments );
		void									executeGetInstanceCommand( ConsoleDevice& device, const std::vector< std::string >& arguments );
	};
}