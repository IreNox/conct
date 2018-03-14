#pragma once

#include "conct_runtime.h"

namespace conct
{
	enum CommandState
	{
		CommandState_Created,
		CommandState_WaitForResponse,
		CommandState_Finish,
	};

	class CommandBase
	{
	public:

						CommandBase( CommandId id );

		CommandState	getState() const { return m_state; }
		CommandId		getId() const { return m_id; }
		ResultId			getResult() const { return m_result; }

		bool			isBusy() const { return m_state == CommandState_WaitForResponse; }
		bool			isFinish() const { return m_state == CommandState_Finish; }
		bool			isOk() const { return m_result == ResultId_Success; }
		bool			hasError() const { return m_result != ResultId_Success; }

	private:

		CommandState	m_state;
		CommandId		m_id;
		ResultId			m_result;
	};

	template< class TData >
	class Command : public CommandBase
	{
	public:

						Command( CommandId id );

		const TData&	getData() const { return m_data; }

	private:

		TData			m_data;
	};
}

#include "conct_command.inl"
