#include "conct_controller.h"

#include "conct_runtime_high.h"

namespace conct
{
	Controller::Controller()
		: m_pRuntime( nullptr )
	{
	}

	Controller::~Controller()
	{

	}

	void Controller::setup( RuntimeHigh* pRuntime )
	{
		m_pRuntime = pRuntime;
	}

	Command< RemoteInstance >* Controller::getInstance( const DeviceAddress& deviceAddress, TypeCrc typeCrc )
	{
		GetInstanceRequest request;
		request.typeCrc = typeCrc;

		ArrayView< uint8 > payload( (uint8*)&request, sizeof( request ) );
		return beginCommand< Command< RemoteInstance > >( deviceAddress, payload );
	}

	Command< Array< RemoteInstance > >* Controller::findInstance( const DeviceAddress& deviceAddress, TypeCrc typeCrc )
	{
		FindInstanceRequest request;
		request.typeCrc = typeCrc;

		ArrayView< uint8 > payload( ( uint8* )&request, sizeof( request ) );
		return beginCommand< Command< Array< RemoteInstance > > >( deviceAddress, payload );
	}

	Command< Value >* Controller::getProperty( const RemoteInstance& instance, const char* pName )
	{
		GetPropertyRequest request;
		request.instanceId = instance.id;

		ArrayView< uint8 > payload( ( uint8* )&request, sizeof( request ) );
		return beginCommand< Command< Value > >( instance.address, payload );
	}

	CommandBase* Controller::setProperty( const RemoteInstance& instance, const char* pName, const Value& value )
	{
		SetPropertyRequest request;
		request.instanceId	= instance.id;
		request.value		= value;

		ArrayView< uint8 > payload( ( uint8* )&request, sizeof( request ) );
		return beginCommand< CommandBase >( instance.address, payload );
	}

	Command< Value >* Controller::callFunction( const RemoteInstance& instance, const char* pName, const Array< Value >& arguments )
	{
		CallFunctionRequest request;
		request.instanceId = instance.id;

		ArrayView< uint8 > payload( ( uint8* )&request, sizeof( request ) );
		return beginCommand< Command< Value > >( instance.address, payload );
	}

	template< class TCommand >
	TCommand* conct::Controller::beginCommand( const DeviceAddress& deviceAddress, const ArrayView< uint8 >& payload )
	{
		CONCT_ASSERT( m_pRuntime != nullptr );

		const DeviceId deviceId = deviceAddress.address[ 0u ];
		TCommand* pCommand = new TCommand( m_pRuntime->getNextCommandId( deviceId ) );

		if( m_pRuntime->sendPackage( pCommand, deviceAddress, payload ) != ResultId_Success )
		{
			delete pCommand;
			pCommand = nullptr;
		}

		return pCommand;
	}
}
