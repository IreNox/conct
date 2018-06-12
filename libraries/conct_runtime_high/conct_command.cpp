#include "conct_command.h"

namespace conct
{
	CommandBase::CommandBase( CommandId id )
	{
		m_state		= CommandState_Created;
		m_id		= id;
		m_result	= ResultId_TimeOut;
	}

	void CommandBase::setSent()
	{
		m_state = CommandState_WaitForResponse;
	}

	void CommandBase::setResponse( ResultId result )
	{
		m_state = CommandState_Finish;
		m_result = result;
	}
}