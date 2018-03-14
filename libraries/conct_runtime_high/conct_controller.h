#pragma once

#include "conct_array.h"
#include "conct_command.h"

namespace conct
{
	class RuntimeHigh;

	class Controller
	{
	public:

											Controller( RuntimeHigh* pRuntime );
											~Controller();

		Command< RemoteInstance >*			getInstance( const DeviceAddress& deviceAddress, TypeCrc typeCrc );
		Command< Array< RemoteInstance > >*	findInstance( const DeviceAddress& deviceAddress, TypeCrc typeCrc );

		Command< Value >*					getProperty( const RemoteInstance& instance, const char* pName );
		Command< Value >*					setProperty( const RemoteInstance& instance, const char* pName, const Value& value );

		Command< Value >*					callFunction( const RemoteInstance& instance, const char* pName, const Array< Value >& arguments );

	private:

		RuntimeHigh*						m_pRuntime;
	};
}
