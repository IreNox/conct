#pragma once

#include "conct_array.h"
#include "conct_array_view.h"
#include "conct_command.h"

#include <vector>

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
		CommandBase*						setProperty( const RemoteInstance& instance, const char* pName, const Value& value );

		Command< Value >*					callFunction( const RemoteInstance& instance, const char* pName, const Array< Value >& arguments );

	private:

		RuntimeHigh*						m_pRuntime;

		template< class TCommand >
		TCommand*							beginCommand( const DeviceAddress& deviceAddress, const ArrayView< uint8 >& payload );
	};
}