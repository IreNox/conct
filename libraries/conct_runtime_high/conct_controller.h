#pragma once

#include "conct_array.h"
#include "conct_array_view.h"
#include "conct_command.h"
#include "conct_value_high.h"
#include "conct_vector.h"

namespace conct
{
	class DataBuilder;
	class RuntimeHigh;

	class Controller
	{
	public:

											Controller();
											~Controller();

		void								setup( RuntimeHigh* pRuntime );
		void								loop();

		Command< ValueHigh >*				getProperty( const RemoteInstance& instance, const char* pName );
		CommandBase*						setProperty( const RemoteInstance& instance, const char* pName, const ValueHigh& value );

		Command< ValueHigh >*				callFunction( const RemoteInstance& instance, const char* pName, const ArrayView< ValueHigh >& arguments );

		void								releaseCommand( CommandBase* pCommand );

	private:

		RuntimeHigh*						m_pRuntime;

		Vector< CommandBase* >				m_releaseCommand;

		template< class TCommand >
		TCommand*							beginCommand( const DeviceAddress& deviceAddress, const DataBuilder& payload, MessageType messageType );
	};
}
