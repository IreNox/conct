#include "conct_command.h"

namespace conct
{
	Command::Command( CommandId id )
	{
		m_state		= CommandState_Created;
		m_id		= id;
		m_result	= ResultId_TimeOut;
	}

	void Command::setSent()
	{
		m_state = CommandState_WaitForResponse;
	}

	void Command::setResponse( ResultId result )
	{
		m_state = CommandState_Finish;
		m_result = result;
	}

	ValueCommand::ValueCommand( CommandId id )
		: Command( id )
	{
	}

	void ValueCommand::setResponse( ResultId result, const ValueHigh& value )
	{
		m_value = value;
		Command::setResponse( result );
	}
}