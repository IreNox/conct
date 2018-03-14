#include "conct_controller.h"

#include "conct_runtime_high.h"

namespace conct
{
	Controller::Controller( RuntimeHigh* pRuntime )
		: m_pRuntime( pRuntime )
	{
	}

	Controller::~Controller()
	{

	}

	Command< RemoteInstance >* Controller::getInstance( const DeviceAddress& deviceAddress, TypeCrc typeCrc )
	{
		Command< RemoteInstance >* pCommand = new Command< RemoteInstance >( m_pRuntime->getNextCommandId( deviceAddress.address[ 0u ] ) );

		std::vector< uint8 > payload;
		// fill payload

		if( m_pRuntime->sendPackage( pCommand, deviceAddress, payload ) != ResultId_Success )
		{
			delete pCommand;
			pCommand = nullptr;
		}

		return pCommand;
	}

	Command< Array< RemoteInstance > >* Controller::findInstance( const DeviceAddress& deviceAddress, TypeCrc typeCrc )
	{

	}

	Command< Value >* Controller::getProperty( const RemoteInstance& instance, const char* pName )
	{

	}

	Command< Value >* Controller::setProperty( const RemoteInstance& instance, const char* pName, const Value& value )
	{

	}

	Command< Value >* Controller::callFunction( const RemoteInstance& instance, const char* pName, const Array< Value >& arguments )
	{

	}
}
