#pragma once

#include "conct_runtime.h"
#include "conct_value_high.h"

namespace conct
{
	class RuntimeHigh;

	enum CommandState
	{
		CommandState_Created,
		CommandState_WaitForResponse,
		CommandState_Finish,
	};

	class Command
	{
		friend class RuntimeHigh;

	public:

						Command( CommandId id );

		CommandState	getState() const { return m_state; }
		CommandId		getId() const { return m_id; }
		ResultId		getResult() const { return m_result; }

		bool			isBusy() const { return m_state == CommandState_WaitForResponse; }
		bool			isFinish() const { return m_state == CommandState_Finish; }
		bool			isOk() const { return m_result == ResultId_Success; }
		bool			isFailure() const { return m_result != ResultId_Success; }

	protected:

		void			setSent();
		void			setResponse( ResultId result );

	private:

		CommandState	m_state;
		CommandId		m_id;
		ResultId		m_result;
	};

	class ValueCommand : public Command
	{
		friend class RuntimeHigh;

	public:

							ValueCommand( CommandId id );

		const ValueHigh&	getValue() const { return m_value; }

	protected:

		void				setResponse( ResultId result, const ValueHigh& data );

	private:

		ValueHigh			m_value;
	};
}
