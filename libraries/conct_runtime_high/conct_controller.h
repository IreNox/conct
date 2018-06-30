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

		ValueCommand*						getProperty( const RemoteInstance& instance, const char* pName );
		Command*							setProperty( const RemoteInstance& instance, const char* pName, const ValueHigh& value );

		ValueCommand*						callFunction( const RemoteInstance& instance, const char* pName, const ArrayView< ValueHigh >& arguments );

		void								releaseCommand( Command* pCommand );

	private:

		RuntimeHigh*						m_pRuntime;

		Vector< Command* >					m_releaseCommand;

		template< class TCommand >
		TCommand*							beginCommand( const DeviceAddress& deviceAddress, const DataBuilder& payload, MessageType messageType );
	};
}
