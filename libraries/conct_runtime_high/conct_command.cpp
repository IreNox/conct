#include "conct_command.h"

namespace conct
{
	CommandBase::CommandBase( CommandId id )
	{
		m_state		= CommandState_Created;
		m_id		= id;
		m_result	= ResultId_Timeout;
	}
}